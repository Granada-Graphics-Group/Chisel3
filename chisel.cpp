#include "chisel.h"

#include "translator.h"
#include "antlr4-runtime.h"
#include "OLexer.h"
#include "OParser.h"
#include "OErrorListener.h"

#include "resourcemanager.h"
#include "renderer.h"
#include "rendertarget.h"
#include "layer.h"
#include "registerlayer.h"
#include "palette.h"
#include "sqlitedatabasemanager.h"
#include "databasetable.h"
#include "logger.hpp"
#include "const.h"

#include "mainwindow.h"
#include "QtUI/openglwidget.h"
#include "QtUI/databaseresourcemodel.h"
#include "QtUI/databasetableschemamodel.h"
#include "QtUI/databasetabledatamodel.h"
#include "QtUI/columnvisibilitymodel.h"
#include "QtUI/visiblecolumnmodel.h"
#include "QtUI/activelayermodel.h"
#include "QtUI/disklayermodel.h"
#include "QtUI/palettelistmodel.h"
#include "QtUI/palettemodel.h"

#include <half.hpp>
#include <QImage>
#include <algorithm>
#include <tuple>
#include <stack>
#include <set>

Chisel::Chisel(MainWindow* mainWindow)
:
    mMainWindow(mainWindow)
{    
    mResourceManager = std::make_unique<ResourceManager>(this, nullptr);
    mResourceManager->setDefaultChiselPath("./temp/");
    
    mDataBaseResourceModel = std::make_unique<DataBaseResourceModel>(mResourceManager->defaultChiselPath());
    
    mDataBaseTableSchemaModel = std::make_unique<DataBaseTableSchemaModel>();
    mDBConnection = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"));
    
    mDataBaseTableDataModel = std::make_unique<DataBaseTableDataModel>(Q_NULLPTR, *(mDBConnection.get()));
    mCurrentRowDataModel = std::make_unique<DataBaseTableDataModel>(Q_NULLPTR, *(mDBConnection.get()));
    mPickedRowDataModel = std::make_unique<DataBaseTableDataModel>(Q_NULLPTR, *(mDBConnection.get()));
    mColumnVisibilityModel = std::make_unique<ColumnVisibilityModel>();    
    mVisibleColumnModel = std::make_unique<VisibleColumnModel>(1, 1);
    
    mDiskLayerModel = std::make_unique<DiskLayerModel>(mResourceManager->defaultChiselPath());
    mActiveLayerModel = std::make_unique<ActiveLayerModel>(mDiskLayerModel.get());
    mPaletteListModel = std::make_unique<PaletteListModel>(mResourceManager.get());
    mPaletteListModel->populateList(mResourceManager->palettes());
    mPaletteListModel->setCurrentLayerType(true);
    mCurrentPalette = mResourceManager->palette(0);
    mPaletteModel = std::make_unique<PaletteModel>(mCurrentPalette);
    mNewPaletteModel = std::make_unique<PaletteModel>();
}

Chisel::~Chisel()
{

}

// *** Public Methods *** //
void Chisel::init()
{
    mRenderer = std::make_unique<Renderer>(RendererType::GL, 1280, 720, mResourceManager.get());
    mRenderer->setChisel(this);
    mResourceManager->setRenderer(mRenderer.get());
    
    //mRenderer->setResourceManager(mResourceManager.get());
    //TODO Create RenderPasses, RenderTargets(ie. PaintingTargets), RenderTechniques
}

SQLiteDatabaseManager* Chisel::databaseManager() const
{
    return mResourceManager->databaseManager();
}

const std::vector<std::string> Chisel::layerNames() const
{
    return mDiskLayerModel->filenames();
}

const std::vector<std::string> Chisel::activeLayerNames(const std::set<Layer::Type>& types) const
{
    std::vector<std::string> layerNames;
    
    for(const auto& layer: mActiveLayers)
        if(types.find(layer->type()) != end(types))
            layerNames.push_back(layer->name());
        
    return layerNames;
}

const std::vector<std::pair<std::string, uint32_t>> Chisel::activeLayerNameAndIndices(const std::set<Layer::Type>& types) const
{
    std::vector<std::pair<std::string, uint32_t>> layerNameAndIndices;

    for(auto i = 0; i < mActiveLayers.size(); ++i)
        if(types.find(mActiveLayers[i]->type()) != end(types))
        layerNameAndIndices.emplace_back(mActiveLayers[i]->name(), i);
        
        
    return layerNameAndIndices;
    
}

Palette* Chisel::palette(std::string name) const
{
    return mResourceManager->palette(name);
}

Palette* Chisel::palette(unsigned int index) const
{
    return mResourceManager->palette(index);
}

std::vector<std::string> Chisel::paletteNames() const
{
    return mResourceManager->paletteNames();
}

std::string Chisel::palettesPath() const
{
    return mResourceManager->palettePath();
}

glm::vec4 Chisel::backgroundColor() const
{
    return mRenderer->backgroundColor();    
}

glm::vec4 Chisel::defaultModelColor() const
{
    return mRenderer->defaultModelColor();
}

glm::vec3 Chisel::specularColor() const
{
    return mRenderer->specularColor();
}

float Chisel::specularPower() const
{
    return mRenderer->specularPower();
}

std::string Chisel::chiselName() const
{
    return mResourceManager->chiselName();
}

bool Chisel::isChiselPathValid() const
{
    return mResourceManager->isChiselPathValid();
}

void Chisel::clearChiselSession()
{
    mActiveLayerModel->removeLayers(0, static_cast<int>(mActiveLayers.size() - 1));
    mActiveLayers.clear();
    mCurrentLayer = nullptr;
    mPaletteListModel->clearLocalPalettes();
}

void Chisel::updateDataBaseResourcePath(const std::string& path)
{
    mDataBaseResourceModel->setNewRootPath(path);
}


void Chisel::updateLayerPath(const std::string& path, bool clearLoadedLayers)
{
    mDiskLayerModel->setNewRootPath(path, clearLoadedLayers);
}

void Chisel::loadChiselFile(std::string name, std::string path)
{
    mResourceManager->loadChiselFile(name, path);
    if(mActiveLayers.size() > 0)
        setCurrentLayer(0);

    mDirtyFlag = false;
}

void Chisel::saveChiselProject(std::string name, std::string path)
{
    mResourceManager->saveChiselProject(name, path);
    
    mDirtyFlag = false;
}

void Chisel::import3DModel(std::string name, std::string extension, std::string path)
{
    mResourceManager->importScene3D(name, extension, path);
    //mResourceManager->createTopology(name);
    
    setCurrentPalette(static_cast<unsigned int>(0));
    
    mDirtyFlag = true;
}

void Chisel::exportChiselModel(std::string filePath, std::string extension, unsigned int layerIndex, bool segmentModel, bool exportCamera)
{
    std::map<std::string, std::vector<uint32_t> > segmentation = (segmentModel) ? segmentModelWithLayer(layerIndex) : std::map<std::string, std::vector<uint32_t> >();

    mResourceManager->exportModel(filePath, extension, mRenderer->scene()->models()[0], segmentation, mRenderer->scene()->camera(), exportCamera);
}

void Chisel::exportChiseProjectToUnity(std::string name, std::string path)
{
    mResourceManager->exportChiselProjectToUnity(name, path);
}

bool Chisel::setDatabase(std::string name)
{
    mDBConnection->setDatabaseName(QString::fromStdString(name));
    auto result = mDBConnection->open();
    return result;
}

Layer* Chisel::createLayer(std::string name, Layer::Type type, std::pair< int, int > resolution)
{
    auto newLayer = mResourceManager->createLayer(name, type, resolution, mResourceManager->palettes()[0]);
    mResourceManager->saveLayer(newLayer);
    addActiveLayer(newLayer);    
    setCurrentLayer(0);
    
    return newLayer;
}

void Chisel::loadLayer(std::string name)
{
    auto loadedLayer = mResourceManager->loadLayer(name);
    if(loadedLayer != nullptr)
    {
        addActiveLayer(loadedLayer);
        setCurrentLayer(0);        
    }
}

void Chisel::duplicateLayer(unsigned int index)
{
    auto newLayer = mResourceManager->duplicateLayer(mActiveLayers[index]);
    mResourceManager->saveLayer(newLayer);    
    insertActiveLayer(index, newLayer);
    //setCurrentLayer(index);
    //setCurrentLayer(index + 1);  
}

void Chisel::unloadLayer(unsigned int index)
{    
    mDiskLayerModel->setFileState(mActiveLayers[index]->name(), false);
    mActiveLayerModel->removeLayers(index, index);
    
    if(mActiveLayers[index]->isDirty()) mResourceManager->saveLayer(mActiveLayers[index]);    
    mResourceManager->unloadLayer(mActiveLayers[index]);
    mRenderer->eraseLayer(index);
        
    mActiveLayers.erase(begin(mActiveLayers) + index);

    if(!mActiveLayers.size()) 
        mCurrentLayer = nullptr;    
}

void Chisel::unloadLayer(std::string name)
{
    auto search = std::find_if(begin(mActiveLayers), end(mActiveLayers), [&](const Layer* currentLayer){ return (name == currentLayer->name()) ? true : false;});

    if(search != end(mActiveLayers))
        unloadLayer(static_cast<unsigned int>(search - begin(mActiveLayers)));
}

void Chisel::unloadTempLayers()
{
    for(auto tempLayer: mTempLayers)
        mResourceManager->unloadLayer(tempLayer);

    mTempLayers.clear();
}

void Chisel::deleteLayer(unsigned int index)
{
    mCurrentLayer = nullptr;

    mActiveLayerModel->removeLayers(index, index);
    
    mResourceManager->deleteLayer(mActiveLayers[index]);
    mRenderer->eraseLayer(index);
    
    mActiveLayers.erase(begin(mActiveLayers) + index);
    
    if(!mActiveLayers.size())
        mCurrentPalette = mResourceManager->palette(0);
}

void Chisel::deleteLayer(std::string name)
{
    auto search = std::find_if(begin(mActiveLayers), end(mActiveLayers), [&](const Layer* currentLayer){ return (name == currentLayer->name()) ? true : false;});

    if(search != end(mActiveLayers))
        deleteLayer(static_cast<unsigned int>(search - begin(mActiveLayers)));
    else
        mResourceManager->deleteDiskLayer(name);
}

void Chisel::saveLayer(unsigned int index)
{
    mResourceManager->saveLayer(mActiveLayers[index]);
}

void Chisel::renameLayer(unsigned int index, const std::string& newName)
{
    auto oldName = mActiveLayers[index]->name();
    mResourceManager->renameLayer(mActiveLayers[index], newName);
    mDiskLayerModel->renameActiveFile(oldName, newName);
    
    mDirtyFlag = true;
}

void Chisel::exportLayerAsImage(unsigned int layerIndex, const std::string& pathName)
{
    mResourceManager->exportLayerAsImage(mActiveLayers[layerIndex], pathName);
}

void Chisel::insertActiveLayer(unsigned int index, Layer* layer)
{        
    mRenderer->insertLayer(index, layer->dataTexture(), layer->maskTexture(), layer->paletteTexture());
    
    auto palette = layer->palette();
    auto paletteTextureData = layer->discrete() ? palette->paletteTextureData(Palette::Type::Discrete) : palette->paletteTextureData(Palette::Type::Linear);        
    mRenderer->updatePaletteTexture(index, paletteTextureData, { palette->minValue(), palette->maxValue() });        
    palette->setDirty(false);

    mDiskLayerModel->setFileState(layer->name(), true);

    mActiveLayers.insert(begin(mActiveLayers) + index, layer);
    mActiveLayerModel->insertLayer(index, QString::fromStdString(layer->name()), layer->type() == Layer::Type::Register, layer->resolution(), layer->typeString());
    mTempFieldLayers.insert(begin(mTempFieldLayers) + index, nullptr);
    
    mDirtyFlag = true;    
}


void Chisel::addActiveLayer(Layer* layer)
{
    mRenderer->insertLayer(0, layer->dataTexture(), layer->maskTexture(), layer->paletteTexture());
    
    auto palette = layer->palette();
    auto paletteTextureData = layer->discrete() ? palette->paletteTextureData(Palette::Type::Discrete) : palette->paletteTextureData(Palette::Type::Linear);        
    mRenderer->updateCurrentPaletteTexture(paletteTextureData, { palette->minValue(), palette->maxValue() });        
    palette->setDirty(false);
    
    mDiskLayerModel->setFileState(layer->name(), true);
    
    mActiveLayers.push_front(layer);
    mActiveLayerModel->addLayer(QString::fromStdString(layer->name()), layer->type() == Layer::Type::Register, layer->resolution(), layer->typeString());
    mTempFieldLayers.push_front(nullptr);
    
    mDirtyFlag = true;
}


void Chisel::propagateNewLayerData()
{  
    if (mCurrentLayer->registerType())
    {
        mDataBaseTableSchemaModel->setDatabaseLayer(static_cast<RegisterLayer*>(mCurrentLayer));
        mDataBaseTableDataModel->setSchema(static_cast<RegisterLayer*>(mCurrentLayer)->tableSchema());
        mCurrentRowDataModel->setSchema(static_cast<RegisterLayer*>(mCurrentLayer)->tableSchema());
        mPickedRowDataModel->setSchema(static_cast<RegisterLayer*>(mCurrentLayer)->tableSchema());
    }
        
    mRenderer->setCurrentPaintTextures(mCurrentLayer->dataTexture(), mCurrentLayer->maskTexture(), mCurrentLayer->paletteTexture());
    mCurrentPalette = mCurrentLayer->palette();
    updatePaletteTexture();
    
    mActiveLayerModel->addLayer(QString::fromStdString(mCurrentLayer->name()), mCurrentLayer->type() == Layer::Type::Register, mCurrentLayer->resolution(), mCurrentLayer->typeString());        
    mPaletteModel->setPalette(mCurrentPalette);
    mPaletteListModel->setCurrentLayerType(mCurrentLayer->discrete());    
}

void Chisel::createLayerAndUpdateInterface(std::string name, Layer::Type type, std::pair<int, int> resolution, int paletteIndex)
{
    mMainWindow->createLayer(QString::fromStdString(name), type, resolution, paletteIndex);
    
    mDirtyFlag = true;
}


void Chisel::setCurrentLayer(unsigned int index)
{
    mCurrentLayerIndex = index;

    if(mCurrentLayer != mActiveLayers[index])
    {
        //if(mTempFieldLayer != nullptr)
        //    mRenderer->setCurrentPaintTextures(mCurrentLayer->dataTexture(), mCurrentLayer->maskTexture(), mCurrentLayer->paletteTexture());
        
        mCurrentLayer = mActiveLayers[index];
        if (mTempFieldLayers[index] == nullptr)
            mCurrentPalette = mCurrentLayer->palette();
        else
            mCurrentPalette = mTempFieldLayers[index]->palette();
        mCurrentLayer->updatePaletteInterpolation();
        mRenderer->setCurrentLayer(index);            
        updatePaletteTexture();

        mPaletteModel->setPalette(mCurrentPalette);
        mPaletteListModel->setCurrentLayerType(mCurrentLayer->discrete());
        
        if(mCurrentLayer->registerType())
        {
            auto regLayer = static_cast<RegisterLayer*>(mCurrentLayer);
            auto tableSchema = regLayer->tableSchema();
            mDataBaseTableSchemaModel->setDatabaseLayer(regLayer);
            mDataBaseTableDataModel->setSchema(tableSchema);
            mDataBaseTableDataModel->setResourceFiles(mResourceManager->layerResourceFiles(regLayer->name()));
            mColumnVisibilityModel->setLayer(regLayer);
            mCurrentRowDataModel->setSchema(tableSchema);
            mCurrentRowDataModel->setResourceFiles(mResourceManager->layerResourceFiles(regLayer->name()));
            mPickedRowDataModel->setSchema(tableSchema);
            mPickedRowDataModel->setResourceDir(mResourceManager->layerResourceDir(regLayer->name()));
            mVisibleColumnModel->setTableSchema(tableSchema);
            mVisibleColumnModel->setColumns(regLayer->primaryColumn(), regLayer->secondaryColumns());
        }
    }
}

void Chisel::changeLayerPosition(int previous, int current)
{
    auto layer = mActiveLayers[previous];
    if(current == static_cast<int>(mActiveLayers.size()))
        mActiveLayers.push_back(layer);
    else
        mActiveLayers.insert(begin(mActiveLayers) + current, layer);
    
    if(current < previous)
        mActiveLayers.erase(begin(mActiveLayers) + previous + 1);
    else
        mActiveLayers.erase(begin(mActiveLayers) + previous);
    
    mRenderer->changeLayerPosition(previous, current);
    
    mDirtyFlag = true;
}

bool Chisel::isLayerDirty(unsigned int index) const
{
    return mActiveLayers[index]->isDirty();
}

bool Chisel::isLayerAreaFieldDirty(unsigned int index) const
{
    auto layer = mActiveLayers[index];
    return (layer->registerType()) ? static_cast<RegisterLayer*>(layer)->isAreaFieldDirty() : false;
}

void Chisel::setLayerDirty(unsigned int index)
{
    if(index < mActiveLayers.size())
    {
        mActiveLayers[index]->setDirty(true);
        mDirtyFlag = true;
    }
}

void Chisel::setLayerAreaFieldDirty(unsigned int index)
{
    if(index < mActiveLayers.size())
    {    
        auto layer = mActiveLayers[index];
        if(layer->registerType())
        {
            auto regLayer = static_cast<RegisterLayer*>(layer);
            if(regLayer->tableSchema()->containFieldType(DataBaseField::Type::Area)) regLayer->setAreaFieldDirty(true);
        }    
    }
}

std::map<std::string, std::vector<uint32_t> > Chisel::segmentModelWithLayer(unsigned int index)
{
    std::map<std::string, std::vector<uint32_t>> segmentation;
    std::vector<std::string> sectionNames;
    
    auto layer = mActiveLayers[index];
    auto isDBLayer = false;
    
    if(layer->registerType())
    {        
        auto regLayer = static_cast<RegisterLayer*>(layer);
        auto result = mResourceManager->databaseManager()->query("SELECT " + regLayer->tableSchema()->field(0).mName + " from " + regLayer->name() + ";");
        for(const auto& row: result) sectionNames.push_back(row[0]);
        isDBLayer = true;
    }
    
    auto texWitdh = layer->dataTexture()->width();
    auto texHeight = layer->dataTexture()->height();
    auto data = mRenderer->readUIntTexture(layer->dataTexture());
    auto mask = mRenderer->readLayerMask(index);
    auto mesh = mRenderer->scene()->models()[0]->mesh();    
    auto uvBuffer = reinterpret_cast<const float *>(mesh->uvBuffer().data());
    auto indexBuffer = reinterpret_cast<const uint32_t *>(mesh->indexBuffer().data());
    
    for(int i = 0; i < mesh->elementCount(); i += 3)
    {
        auto index1 = 2 * indexBuffer[i];
        auto index2 = 2 * indexBuffer[i + 1];
        auto index3 = 2 * indexBuffer[i + 2];
        
        auto uv1 = glm::ivec2(uvBuffer[index1] * texWitdh, uvBuffer[index1 + 1] * texHeight);
        auto uv2 = glm::ivec2(uvBuffer[index2] * texWitdh, uvBuffer[index2 + 1] * texHeight);
        auto uv3 = glm::ivec2(uvBuffer[index3] * texWitdh, uvBuffer[index3 + 1] * texHeight);
        
        auto mask1 = mask[uv1.y * texHeight + uv1.x];
        auto mask2 = mask[uv2.y * texHeight + uv2.x];
        auto mask3 = mask[uv3.y * texHeight + uv3.x];
        
        auto value1 = data[uv1.y * texHeight + uv1.x];
        auto value2 = data[uv2.y * texHeight + uv2.x];
        auto value3 = data[uv3.y * texHeight + uv3.x];

        std::string segmentName = "Unused";
        
        if(value1 == value2 && value1 == value3)
        {
            if(mask1 > 0)
                segmentName = (isDBLayer) ? sectionNames[value1] : std::to_string(value1);
        }
        else
        {
            auto centroid = (uv1 + uv2 + uv3)/3;
            auto centroidMask = mask[centroid.y * texHeight + centroid.x];
            auto centroidValue = data[centroid.y * texHeight + centroid.x];
            
            if(centroidMask > 0)
                segmentName = (isDBLayer) ? sectionNames[centroidValue] : std::to_string(centroidValue);            
        }
        
        segmentation[segmentName].push_back(index1 * 0.5);
        segmentation[segmentName].push_back(index2 * 0.5);
        segmentation[segmentName].push_back(index3 * 0.5);        
    }
    
    return segmentation;
}

std::vector<float> Chisel::computeAreaPerCell(std::pair<int, int> layerResolution)
{
    auto areaTexture = mRenderer->computeAreaPerTexelTexture(layerResolution);
    auto areaData = mRenderer->readFloatTexture(areaTexture);
    //mResourceManager->deleteTexture(areaTexture);
    
    return areaData;
}


float Chisel::computeSurfaceArea()
{
    auto data = computeAreaPerCell(mCurrentLayer->resolution());
//     auto areaTexture = mResourceManager->texture("AreaPerPixel");
//     auto data = mRenderer->readFloatTexture(areaTexture);
    
    double area = 0;

    for (auto i = 0; i < data.size(); ++i)
            area += data[i];

    return static_cast<float>(area);
}

float Chisel::computeLayerArea(unsigned int index)
{
//     auto areaTexture = mResourceManager->texture("AreaPerPixel");
//     auto data = mRenderer->readFloatTexture(areaTexture);
    auto data = computeAreaPerCell(mCurrentLayer->resolution());
    auto mask = mRenderer->readLayerMask(index);

    double area = 0;

    for (int i = 0; i < data.size(); ++i)
        if (mask[i] != 0.0)
            area += data[i];

    return static_cast<float>(area);
}

std::vector<std::array<float, 2>> Chisel::computeLayerValueArea(unsigned int index)
{
    auto layer = mActiveLayers[index];
    auto mask = mRenderer->readLayerMask(index);

//     auto areaTexture = mResourceManager->texture("AreaPerPixel");
//     auto areaData = mRenderer->readFloatTexture(areaTexture);
    auto areaData = computeAreaPerCell(mCurrentLayer->resolution());

    std::map<float, double> mappedAreaPerValue;
    std::vector<std::array<float, 2>> areaPerValue;

    switch (layer->dataTexture()->type())
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        {
            auto data = mRenderer->readCharTexture(layer->dataTexture());

            for (auto i = 0; i < data.size(); ++i)
                if (mask[i] > 0)
                    mappedAreaPerValue[data[i]] += areaData[i];
            break;
        }
        case GL_SHORT:
        {
            auto data = mRenderer->readShortTexture(layer->dataTexture());

            for (auto i = 0; i < data.size(); ++i)
                if (mask[i] > 0)
                    mappedAreaPerValue[data[i]] += areaData[i];
            break;
        }
        case GL_UNSIGNED_SHORT:
        {
            auto data = mRenderer->readUShortTexture(layer->dataTexture());

            for (auto i = 0; i < data.size(); ++i)
                if (mask[i] > 0)
                    mappedAreaPerValue[data[i]] += areaData[i];
            break;
        }
        case GL_INT:
        {
            auto data = mRenderer->readIntTexture(layer->dataTexture());

            for (auto i = 0; i < data.size(); ++i)
                if (mask[i] > 0)
                    mappedAreaPerValue[data[i]] += areaData[i];
            break;
        }
        case GL_UNSIGNED_INT:
        {
            auto data = mRenderer->readUIntTexture(layer->dataTexture());

            for (auto i = 0; i < data.size(); ++i)
                if (mask[i] > 0)
                    mappedAreaPerValue[data[i]] += areaData[i];
            break;
        }
        case GL_FLOAT:
        {
            auto data = mRenderer->readFloatTexture(layer->dataTexture());

            for (auto i = 0; i < data.size(); ++i)
                if (mask[i] > 0)
                    mappedAreaPerValue[data[i]] += areaData[i];
            break;
        }
    }

    for(const auto& valuePair: mappedAreaPerValue)
        areaPerValue.push_back({{valuePair.first, static_cast<float>(valuePair.second)}});
    
    return areaPerValue;
}

void Chisel::updateTableAreaFields(unsigned int index)
{
    auto layer = mActiveLayers[index];
    
    if(layer->registerType())
    {
        auto dbLayer = dynamic_cast<RegisterLayer *>(layer);                
        if(dbLayer->tableSchema()->containFieldType(DataBaseField::Type::Area) && dbLayer->isAreaFieldDirty())
        {
            auto areaPerValue = computeLayerValueArea(index);
            mDataBaseTableDataModel->setAreaFields(areaPerValue);
            dbLayer->setAreaFieldDirty(false);
        }        
    }
}

std::vector<float> Chisel::computeTopology(std::pair<int, int> layerResolution)
{
    auto topologyTexture = mRenderer->computeTopologyTexture(layerResolution);
    auto data = mRenderer->readFloatTexture(topologyTexture);
    
    for (int i = 0; i < data.size(); ++i)
        if (data[i] != 0)
            auto a = 1;

    return data;
}


Layer* Chisel::computeCellAreaLayer(std::string layerName, const std::pair<int, int> layerResolution)
{    
    //auto areaTexture = mResourceManager->texture("AreaPerPixel");
    //auto copyAreaTexture = mResourceManager->copyTexture(areaTexture, layerName + ".Data");
    
    std::vector<glm::byte> maskTextureData(layerResolution.first * layerResolution.second, 255);
    std::vector<glm::byte> valueTextureData(layerResolution.first * layerResolution.second * 4, 0);
    
    auto cellAreaLayer = mResourceManager->createLayer(layerName, Layer::Type::Float32, layerResolution, valueTextureData, maskTextureData, mResourceManager->palette(0));    
    addActiveLayer(cellAreaLayer);

    mRenderer->computeLayerOperation(3);

    mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));
    mRenderer->padLayerTextures(0);
    
    return cellAreaLayer;
}

std::vector<std::array<double, 2>> Chisel::computeAreaStatistics(unsigned int functionLayerIndex, int functionFieldIndex, unsigned int baseLayerIndex, StatOps operation)
{
    auto functionLayer = mActiveLayers[functionLayerIndex];
    Layer* fieldLayer = nullptr;
    std::vector<double> functionData;

    mRenderer->updateLayerSizeDependentEditingResources(functionLayer->resolution());
    mRenderer->updateAreaAndTopologyTechs(functionLayer->resolution());
    mGLWidget->repaint();
    
    if(functionLayer->registerType() && functionFieldIndex > -1)
    {
        auto regLayer = static_cast<RegisterLayer*>(functionLayer);
        fieldLayer = mResourceManager->createLayerFromTableField(regLayer, regLayer->field(functionFieldIndex));
        mMainWindow->visualizer()->update();
        functionData = mRenderer->readLayerDataTexture(fieldLayer->dataTexture());
    }
    else
        functionData = mRenderer->readLayerData(functionLayerIndex);
        
    auto functionMask = mRenderer->readLayerMask(functionLayerIndex);
    
    auto baseLayer = mActiveLayers[baseLayerIndex];    
    auto baseData = mRenderer->readLayerData(baseLayerIndex);
    auto baseMask = mRenderer->readLayerMask(baseLayerIndex);
    
    
    std::unordered_map<double, double> areaStatistics;    
    std::vector<std::array<double, 2>> doubleAreaStatistics;
    
    switch(operation)
    {
        case StatOps::MeanValue:
        {   
            std::unordered_map<double, uint64_t> count;
            
            for(auto i = 0; i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                {
                    areaStatistics[baseData[i]] += functionData[i];
                    count[baseData[i]]++;
                }
            
            for (auto& statPair : areaStatistics)//(auto& [key, value] : areaStatistics)
                statPair.second = statPair.second/count[statPair.first];//value = value/count[key];
        }
            break;        
        case StatOps::MinValue:
        {   
            for(auto i = 0; i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                {
                    auto category = baseData[i];
                    if(!areaStatistics.count(category)) areaStatistics[category] = std::numeric_limits<int64_t>::max();
                    if(areaStatistics[category] > functionData[i]) areaStatistics[category] = functionData[i];
                }
        }            
            break;
        case StatOps::MaxValue:
        {
            for(auto i = 0; i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                {
                    auto category = baseData[i];
                    if(!areaStatistics.count(category)) areaStatistics[category] = std::numeric_limits<int64_t>::min();
                    if(areaStatistics[category] < functionData[i]) areaStatistics[category] = functionData[i];
                }            
        }                        
            break;
        case StatOps::Variance:
        {
            std::unordered_map<double, uint64_t> count;
            std::unordered_map<double, double> mean;
            
            for(auto i = 0; i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                {
                    mean[baseData[i]] += functionData[i];
                    count[baseData[i]]++;
                }
            
            for (auto& meanPair : mean)//(auto& [key, value] : mean)
                meanPair.second = meanPair.second / count[meanPair.first];//value = value/count[key];
            
            for(auto i = 0; i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                {
                    auto tempValue = functionData[i] - mean[baseData[i]];
                    areaStatistics[baseData[i]] = tempValue * tempValue;
                }

            for (auto& statPair : areaStatistics)//(auto& [key, value] : areaStatistics)
                statPair.second = statPair.second / count[statPair.first];//value = value/count[key];          
        }
            break;
            
        case StatOps::StdDeviation:
        {
            std::unordered_map<double, uint64_t> count;
            std::unordered_map<double, double> mean;
            
            for(auto i = 0; i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                {
                    mean[baseData[i]] += functionData[i];
                    count[baseData[i]]++;
                }
            
            for (auto& meanPair : mean)//(auto& [key, value] : mean)
                meanPair.second = meanPair.second / count[meanPair.first];//value = value/count[key];
            
            for(auto i = 0; i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                {
                    auto tempValue = functionData[i] - mean[baseData[i]];
                    areaStatistics[baseData[i]] = tempValue * tempValue;
                }
            
            for (auto& statPair : areaStatistics)//(auto& [key, value] : areaStatistics)
                statPair.second = std::sqrt(statPair.second / count[statPair.first]);//value = std::sqrt(value/count[key]);
        }
            break;
        case StatOps::NoNull:
        {
            auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
            
            for(auto i = 0;i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] != 0)
                    areaStatistics[baseData[i]] += areaData[i];            
        }
            break;
        case StatOps::Null:
        {
            auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));

            for(auto i = 0;i < functionData.size(); ++i)
                if(baseMask[i] != 0 && functionMask[i] == 0)
                    areaStatistics[baseData[i]] += areaData[i];            
        }
            break;
    }
    
    for(const auto& stat : areaStatistics)
        doubleAreaStatistics.push_back({stat.first, stat.second});
    
    if(baseLayer->registerType())
    {
        auto regLayer = static_cast<RegisterLayer*>(baseLayer);

        DataBaseTable* currentSchema = nullptr;
        if(mCurrentLayer != baseLayer)
        {
            if(mCurrentLayer->registerType())
                currentSchema = mDataBaseTableDataModel->schema();

            mDataBaseTableDataModel->setSchema(regLayer->tableSchema());
        }
        
        if(!regLayer->containField("Statistics"))
            mDataBaseTableDataModel->addDoubleField("Statistics", doubleAreaStatistics);
        else
            mDataBaseTableDataModel->setFieldData("Statistics", doubleAreaStatistics);
        
        if(currentSchema)
            mDataBaseTableDataModel->setSchema(currentSchema);
    }
    
    if(fieldLayer != nullptr)
        mResourceManager->deleteLayer(fieldLayer);
//     std::unordered_map<layerDataTypes, double> areaStatistics;
//         
//     switch(operation)
//     {
//         case AreaStatOps::MeanValue:
//         {            
//             std::unordered_map<layerDataTypes, int64_t> count;
//             
//             auto sumAndCount = [&areaStatistics, &count](auto&& function, auto&& base){ areaStatistics[base] += function; count[base]++;};
//             
//             for(auto i = 0; i < functionData.size(); ++i)
//                 if(baseMask[i] != 0 && functionMask[i] != 0)
//                     std::visit(sumAndCount, functionData[i], baseData[i]);
//             
//             for(auto& [key, value] : areaStatistics)
//                value = value/count[key];
//         }
//             break;
//         case AreaStatOps::MinValue:
//         {            
//             auto min = [&areaStatistics](auto&& function, auto&& base)
//             { 
//                 if(!areaStatistics.count(base)) areaStatistics[base] = std::numeric_limits<int64_t>::max();
//                 if(areaStatistics[base] > function) areaStatistics[base] = function;                
//             };
//             
//             for(auto i = 0; i < functionData.size(); ++i)
//                 if(baseMask[i] != 0 && functionMask[i] != 0)
//                     std::visit(min, functionData[i], baseData[i]);
//         }            
//             break;
//         case AreaStatOps::MaxValue:
//         {            
//             auto max = [&areaStatistics](auto&& function, auto&& base)
//             { 
//                 if(!areaStatistics.count(base)) areaStatistics[base] = std::numeric_limits<int64_t>::min();
//                 if(areaStatistics[base] < function) areaStatistics[base] = function;                
//             };
//             
//             for(auto i = 0; i < functionData.size(); ++i)
//                 if(baseMask[i] != 0 && functionMask[i] != 0)
//                     std::visit(max, functionData[i], baseData[i]);
//         }                        
//             break;
//         case AreaStatOps::Variance:
//         {
//             std::unordered_map<layerDataTypes, int64_t> count;
//             std::unordered_map<layerDataTypes, double> mean;
//             
//             auto sumAndCount = [&mean, &count](auto&& function, auto&& base){ mean[base] += function; count[base]++;};
//             
//             for(auto i = 0; i < functionData.size(); ++i)
//                 if(baseMask[i] != 0 && functionMask[i] != 0)
//                     std::visit(sumAndCount, functionData[i], baseData[i]);
//             
//             for(auto& [key, value] : mean)
//                value = value/count[key];
//             
//             auto const addSquare = [&areaStatistics, &mean](auto&& function, auto&& base)
//             {
//                 auto d = function - mean[base];
//                 areaStatistics[base] += d * d;
//             };
//             
//             for(auto i = 0; i < functionData.size(); ++i)
//                 if(baseMask[i] != 0 && functionMask[i] != 0)
//                     std::visit(addSquare, functionData[i], baseData[i]);            
//             
//             for(auto& [key, value] : areaStatistics)
//                value = value/count[key];
//         }
//             break;
//             
//         case AreaStatOps::StdDeviation
//         {
//             
//         }
//         break;
//         case AreaStatOps::NoNull:
//         {
//             auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
//             
//             std::size_t i = 0;
//             auto add = [&areaStatistics, &areaData, &i](auto&& base){ areaStatistics[base] += areaData[i]; };
//             
//             for(;i < functionData.size(); ++i)
//                 if(baseMask[i] != 0 && functionMask[i] != 0)
//                     std::visit(add, baseData[i]);
//         }
//             break;
//         case AreaStatOps::Null:
//         {
//             auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
//             
//             std::size_t i = 0;
//             auto add = [&areaStatistics, &areaData, &i](auto&& base){ areaStatistics[base] += areaData[i]; };
//             
//             for(;i < functionData.size(); ++i)
//                 if(baseMask[i] != 0 && functionMask[i] == 0)
//                     std::visit(add, baseData[i]);
//         }
//             break;
//     }
//     
//     std::vector<std::array<double, 2>> doubleAreaStatistics;
//     
//     auto toDouble = [](auto&& arg) -> double { return arg; };
//     
//     for(const auto& stat : areaStatistics)
//         doubleAreaStatistics.push_back({std::visit(toDouble, stat.first), stat.second});
        
    return doubleAreaStatistics;
}

Layer* Chisel::computeNeighborhoodStatistics(std::string layerName, unsigned int functionLayerIndex, int functionFieldIndex, unsigned int radius, StatOps operation)
{
    auto functionLayer = mActiveLayers[functionLayerIndex];
    std::vector<double> functionData;

    mRenderer->updateLayerSizeDependentEditingResources(functionLayer->resolution());
    mRenderer->updateAreaAndTopologyTechs(functionLayer->resolution());
    mGLWidget->repaint();
    
    if(functionLayer->registerType() && functionFieldIndex > -1)
    {
        auto regLayer = static_cast<RegisterLayer*>(functionLayer);
        auto fieldLayer = mResourceManager->createLayerFromTableField(regLayer, regLayer->field(functionFieldIndex));
        mMainWindow->visualizer()->update();
        functionData = mRenderer->readLayerDataTexture(fieldLayer->dataTexture());
        mResourceManager->deleteLayer(fieldLayer);
    }
    else
        functionData = mRenderer->readLayerData(functionLayerIndex);
        
    auto functionMask = mRenderer->readLayerMask(functionLayerIndex);

    auto neighborhoodTexture = mResourceManager->texture("Neighborhood");    
    auto neighborhoodData = mRenderer->readFloatTexture(neighborhoodTexture);
    
    std::vector<float> neighborhoodStatsData(functionData.size(), 0);
    std::vector<glm::byte> valueTextureData(functionData.size() * sizeof(float), 0);
    std::vector<glm::byte> maskTextureData(functionData.size(), 0);

    auto width = functionLayer->dataTexture()->width();
    auto height = functionLayer->dataTexture()->height();
    auto texelCount = width * height;
    std::array<int, 8> neighborOffsets = { -1, 1, width, -width, -1 + width, -1 - width, 1 + width, 1 - width };
    
    std::vector<uint64_t> surfaceCostData(functionData.size(), std::numeric_limits<uint64_t>::max());            
    std::vector<char> inspectedTexels(functionData.size(), 2);

    for(auto i = 0; i < functionMask.size(); ++i)
        if(functionMask[i] && neighborhoodData[2 * i] < 0)
        {
            std::vector<double> statisticsData;
            std::vector<uint64_t> dirtyIndices;
            std::set<std::pair<uint64_t, uint64_t>> activeTexels;
            
            activeTexels.insert({0,i});
            surfaceCostData[i] = 0;
            inspectedTexels[i] = 1;
                        
            do
            {
                auto currentTexelIndex = activeTexels.cbegin()->second;
                activeTexels.erase(activeTexels.cbegin());
                
                if(functionMask[currentTexelIndex])
                {
                    /*if(surfaceCostData[currentTexelIndex] < radius)     */               
                        for(int idx = 0; idx < 8; ++idx)
                        {
                            int neighborTexelIndex = currentTexelIndex + neighborOffsets[idx];

                            if (neighborTexelIndex > 0 && neighborTexelIndex < texelCount)
                            {
                                int neighborhoodDataValue = neighborhoodData[2 * currentTexelIndex];

                                if (neighborhoodDataValue >= 0 || (-neighborhoodDataValue & (1 << idx)) == 0)
                                {
                                    int indirectIndex = 2 * neighborTexelIndex;
                                    neighborTexelIndex = neighborhoodData[indirectIndex + 1] * width + neighborhoodData[indirectIndex];

                                    auto validTexel = neighborTexelIndex > 0 && neighborTexelIndex < texelCount;
                                    if (!validTexel || validTexel && neighborhoodData[2 * neighborTexelIndex] > 0)
                                        neighborTexelIndex = 0;
                                }
                            }
                            else
                                neighborTexelIndex = 0;

                            if (neighborTexelIndex != 0)
                            {
                                float costToNeighbor = surfaceCostData[currentTexelIndex] + 1;
                                                            
                                if(costToNeighbor <= radius && surfaceCostData[neighborTexelIndex] > costToNeighbor)
                                {
                                    if(inspectedTexels[neighborTexelIndex] > 1)
                                    {
                                        activeTexels.insert({costToNeighbor, neighborTexelIndex});
                                        inspectedTexels[neighborTexelIndex] = 1;
                                    }
                                    else if(inspectedTexels[neighborTexelIndex] == 1)
                                    {
                                        auto found = activeTexels.find({surfaceCostData[neighborTexelIndex], neighborTexelIndex});
                                        if(found != end(activeTexels))
                                        {
                                            activeTexels.erase(found);
                                            activeTexels.insert({costToNeighbor, neighborTexelIndex});
                                        }                                
                                    }
                                    
                                    if(neighborhoodData[2 * neighborTexelIndex] == 0 && neighborhoodData[2 * neighborTexelIndex + 1] == 0)
                                        auto a = 0;
                                    else
                                    {
                                        surfaceCostData[neighborTexelIndex] = costToNeighbor;
                                        dirtyIndices.push_back(neighborTexelIndex);
                                    }
                                }
                            }
                        }
                    
                    inspectedTexels[currentTexelIndex] = 0;                                
                    statisticsData.push_back(functionData[currentTexelIndex]);
                    dirtyIndices.push_back(currentTexelIndex);
                }
            }while(!activeTexels.empty());

            for(const auto& index: dirtyIndices)
            {
                surfaceCostData[index] = std::numeric_limits<uint64_t>::max();
                inspectedTexels[index] = 2;
            }

            maskTextureData[i] = 255;
            
            switch(operation)
            {
                case StatOps::MeanValue:
                {
                    neighborhoodStatsData[i] = std::accumulate(begin(statisticsData), end(statisticsData), 0.0)/ statisticsData.size();
                    auto b = neighborhoodStatsData[i];
                    auto c = 0;
                }
                break;
                case StatOps::MinValue:
                {
                    neighborhoodStatsData[i] = *std::min_element(begin(statisticsData), end(statisticsData));
                }
                break;
                case StatOps::MaxValue:
                {
                    neighborhoodStatsData[i] = *std::max_element(begin(statisticsData), end(statisticsData));
                }
                break;
                case StatOps::Variance:
                {
                    auto mean = std::accumulate(begin(statisticsData), end(statisticsData), 0.0) / statisticsData.size();
                    
                    auto const add_square = [&mean](double sum, double i)
                    {                        
                        auto d = i - mean;
                        return sum + d * d;
                    };
                                        
                    neighborhoodStatsData[i] = std::accumulate(begin(statisticsData), end(statisticsData), 0.0, add_square) / (statisticsData.size() - 1);
                }
                break;
                case StatOps::StdDeviation:
                {
                    auto mean = std::accumulate(begin(statisticsData), end(statisticsData), 0.0) / statisticsData.size();
                    
                    auto const add_square = [&mean](double sum, double i)
                    {                        
                        auto d = i - mean;
                        return sum + d * d;
                    };
                                        
                    auto variance = std::accumulate(begin(statisticsData), end(statisticsData), 0.0, add_square) / (statisticsData.size() - 1);
                    
                    neighborhoodStatsData[i] = std::sqrt(variance);
                }
                break;
                case StatOps::NoNull:
                {
                    //auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
                    
                }
                break;
                case StatOps::Null:
                {
                    //auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
                }
                break;
            }  
    }
    
    auto statsByteData = reinterpret_cast<glm::byte*>(neighborhoodStatsData.data());
    std::copy(statsByteData, statsByteData + valueTextureData.size(), begin(valueTextureData));
        
    auto neighborhoodStatsLayer = mResourceManager->createLayer(layerName, Layer::Type::Float32, functionLayer->resolution(), valueTextureData, maskTextureData, functionLayer->palette());
    
    addActiveLayer(neighborhoodStatsLayer);        
    mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));
    mRenderer->padLayerTextures(0);

    return neighborhoodStatsLayer;
}

Layer* Chisel::computeCostSurfaceLayer(std::string layerName, unsigned int seedLayerIndex, unsigned int costLayerIndex, double maxCost)
{
    auto seedLayer = mActiveLayers[seedLayerIndex];
    auto seedTexture = seedLayer->maskTexture();

    mRenderer->updateLayerSizeDependentEditingResources(seedLayer->resolution());
    mRenderer->updateAreaAndTopologyTechs(seedLayer->resolution());
    mGLWidget->repaint();

    auto neighborhoodTexture = mResourceManager->texture("Neighborhood");

    auto seedData = mRenderer->readLayerMask(seedLayerIndex);
    auto frictionData = mRenderer->readLayerData(costLayerIndex);
    auto neighborhoodData = mRenderer->readFloatTexture(neighborhoodTexture);
    std::vector<float> surfaceCostData(frictionData.size(), std::numeric_limits<float>::max());
    
    std::vector<glm::byte> valueTextureData(surfaceCostData.size() * sizeof(float), 0);
    std::vector<glm::byte> maskTextureData(surfaceCostData.size(), 0);
    
    std::set<std::pair<double, uint64_t>> activeTexels;
    std::vector<char> inspectedTexels(frictionData.size(), 2);
    std::array<int, 8> neighborOffsets = { -1, 1, seedTexture->width(), -seedTexture->width(), -1 + seedTexture->width(), -1 - seedTexture->width(), 1 + seedTexture->width(), 1 - seedTexture->width() };
    
    for(auto i = 0; i < seedData.size(); ++i)
        if(seedData[i] && neighborhoodData[2 * i] < 0)
        {
            activeTexels.insert({0,i});
            surfaceCostData[i] = 0.0f;
            inspectedTexels[i] = 1;
        }
    
    if(activeTexels.size())
    {
        auto texelCount = seedLayer->width() * seedLayer->height();
        auto validateTexel = [&texelCount](const auto& texel) { return texel > 0 && texel < texelCount; };

        do
        {
            auto currentTexelIndex = activeTexels.cbegin()->second;
            activeTexels.erase(activeTexels.cbegin());

            for (int i = 0; i < 8; ++i)
            {
                int neighborTexelIndex = currentTexelIndex + neighborOffsets[i];

                if (validateTexel(neighborTexelIndex))
                {
                    int neighborhoodDataValue = neighborhoodData[2 * currentTexelIndex];

                    if (neighborhoodDataValue >= 0 || (-neighborhoodDataValue & (1 << i)) == 0)
                    {
                        int indirectIndex = 2 * neighborTexelIndex;
                        neighborTexelIndex = neighborhoodData[indirectIndex + 1] * seedTexture->width() + neighborhoodData[indirectIndex];

                        auto validTexel = validateTexel(neighborTexelIndex);
                        if (!validTexel || validTexel && neighborhoodData[2 * neighborTexelIndex] > 0)
                            neighborTexelIndex = 0;
                    }
                }
                else
                    neighborTexelIndex = 0;

                if (neighborTexelIndex != 0)
                {
                    float costToNeighbor = surfaceCostData[currentTexelIndex];
                    
                    if(i < 4)
                        costToNeighbor += std::sqrt(frictionData[currentTexelIndex])/2.0 + std::sqrt(frictionData[neighborTexelIndex])/2.0;
                    else
                        costToNeighbor += std::sqrt(2 * frictionData[currentTexelIndex])/2.0 + std::sqrt(2 * frictionData[neighborTexelIndex])/2.0;
                        
                    if(costToNeighbor <= maxCost && surfaceCostData[neighborTexelIndex] > costToNeighbor)
                    {
                        if(inspectedTexels[neighborTexelIndex] > 1)
                        {
                            activeTexels.insert({costToNeighbor, neighborTexelIndex});
                            inspectedTexels[neighborTexelIndex] = 1;
                        }
                        else if(inspectedTexels[neighborTexelIndex] == 1)
                        {
                            auto found = activeTexels.find({surfaceCostData[neighborTexelIndex], neighborTexelIndex});
                            if(found != end(activeTexels))
                            {
                                activeTexels.erase(found);
                                activeTexels.insert({costToNeighbor, neighborTexelIndex});
                            }
                            
                        }
                        
                        if(neighborhoodData[2 * neighborTexelIndex] == 0 && neighborhoodData[2 * neighborTexelIndex + 1] == 0)
                            auto a = 0;
                        else
                            surfaceCostData[neighborTexelIndex] = costToNeighbor;                
                    }
                }
            }
            
            inspectedTexels[currentTexelIndex] = 0;
            maskTextureData[currentTexelIndex] = 255;
        }while(!activeTexels.empty());
        
        auto surfaceCostByteData = reinterpret_cast<glm::byte*>(surfaceCostData.data());
        std::copy(surfaceCostByteData, surfaceCostByteData + surfaceCostData.size() * sizeof(float), begin(valueTextureData));
        
        auto surfaceCostLayer = mResourceManager->createLayer(layerName, Layer::Type::Float32, seedLayer->resolution(), valueTextureData, maskTextureData, seedLayer->palette());
        addActiveLayer(surfaceCostLayer);
        
        mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));

        mRenderer->padLayerTextures(0);

        return surfaceCostLayer;
    }
    
    return nullptr;    
}


Layer* Chisel::computeDistanceFieldLayer(std::string layerName, unsigned int index, double distance)
{
    //qApp->processEvents();
    auto seedLayer = mActiveLayers[index];
    auto seedTexture = seedLayer->maskTexture();
    
    mRenderer->updateLayerSizeDependentEditingResources(seedLayer->resolution());
    mRenderer->updateAreaAndTopologyTechs(seedLayer->resolution());
    mGLWidget->repaint();

    auto areaTexture = mResourceManager->texture("AreaPerPixel");
    auto neighborhoodTexture = mResourceManager->texture("Neighborhood");

    auto seedData = mRenderer->readLayerMask(index);
    auto areaData = mRenderer->readFloatTexture(areaTexture);//computeAreaPerCell(seedLayer->resolution());
    auto neighborhoodData = mRenderer->readFloatTexture(neighborhoodTexture);//computeTopology(seedLayer->resolution());
    std::vector<float> distanceData(areaData.size(), std::numeric_limits<float>::max());
    std::vector<float> distanceDataX(areaData.size(), std::numeric_limits<float>::max());
    
    std::vector<glm::byte> valueTextureData(distanceData.size() * sizeof(float), 0);
    std::vector<glm::byte> maskTextureData(distanceData.size(), 0);
    std::vector<glm::byte> maskTextureDataX(distanceData.size(), 0);
    
    std::set<std::pair<double, uint64_t>> activeTexels;
    std::vector<char> inspectedTexels(areaData.size(), 2);
    //glm::ivec2 immediateOffsets[8] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}};    
    //std::array<int, 8> neighborOffsets = { -seedTexture->width(), seedTexture->width(), 1, -1, -seedTexture->width() + 1, -seedTexture->width() - 1, seedTexture->width() + 1, seedTexture->width() - 1 };
    std::array<int, 8> neighborOffsets = { -1, 1, seedTexture->width(), -seedTexture->width(), -1 + seedTexture->width(), -1 - seedTexture->width(), 1 + seedTexture->width(), 1 - seedTexture->width() };
    

    auto normalLayers = computeNormalLayer("Normals", seedLayer->resolution());    
    auto posLayers = computePositionLayer("Position", seedLayer->resolution());
    mGLWidget->repaint();
    
    std::array<std::vector<float>, 3> normalData;
    normalData[0] = mRenderer->readFloatTexture(normalLayers[0]->dataTexture());
    normalData[1] = mRenderer->readFloatTexture(normalLayers[1]->dataTexture());
    normalData[2] = mRenderer->readFloatTexture(normalLayers[2]->dataTexture());
    auto normalMask = mRenderer->readTexture(normalLayers[0]->maskTexture());
    
    std::array<std::vector<float>, 3> posData;
    posData[0] = mRenderer->readFloatTexture(posLayers[0]->dataTexture());
    posData[1] = mRenderer->readFloatTexture(posLayers[1]->dataTexture());
    posData[2] = mRenderer->readFloatTexture(posLayers[2]->dataTexture());        
        
        
    std::vector<glm::vec3> positionTexels; //{{0, 1, 0}, {0.3, -1, 0.8}, {0.8, 0.8, 1}, {-1, 0.7, 0}};
    std::vector<glm::vec3> normalTexels; //{{0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {-1, 0, 0}};
    
    for(auto i = 0; i < normalMask.size(); ++i)
    {
        glm::vec3 pos{posData[0][i], posData[1][i], posData[2][i]};
        glm::vec3 norm{std::rint(normalData[0][i]), std::rint(normalData[1][i]), std::rint(normalData[2][i])};       
        
        if(norm.x > -0.5 && norm.x < 0.5) norm.x = std::abs(norm.x);
        if(norm.y > -0.5 && norm.y < 0.5) norm.y = std::abs(norm.y);
        if(norm.z > -0.5 && norm.z < 0.5) norm.z = std::abs(norm.z);
                
        positionTexels.push_back(pos);
        normalTexels.push_back(norm);
        
    }

    auto originIndex = -1;
    for(auto i = 0; i < seedData.size(); ++i)
        if(seedData[i] && neighborhoodData[2 * i] < 0 && originIndex == -1)
        {
            auto v = 1762181;
            if (seedTexture->width() == 4096)
                v = 7046922;
            else if (seedTexture->width() == 8192)
                v = 28184084;
            
            activeTexels.insert({0,v});
            distanceData[v] = 0.0f;
            inspectedTexels[v] = 1;
            originIndex = v;
        }

    
    auto originPosition = positionTexels[originIndex];
    auto originNormal = normalTexels[originIndex];

    for(auto i = 1; i < positionTexels.size(); ++i)
        if(normalMask[i] && i != originIndex)
        {
            auto position = positionTexels[i];
            auto faceNormal = normalTexels[i];
            
            if(originNormal == faceNormal)
            {
                distanceDataX[i] = glm::distance(originPosition, position); 
            }
            else if (originNormal + faceNormal != glm::vec3(0)) // contiguos faces
            {
                auto rotationAxis = glm::cross(originNormal, faceNormal);

                auto lower = originNormal + faceNormal - rotationAxis;
                auto higher = originNormal + faceNormal + rotationAxis;
                
                auto halfSize = 1.0;
                
                auto positionOnOriginNormal = position * glm::abs(originNormal) - originPosition * glm::abs(originNormal);
                auto distance = glm::abs((positionOnOriginNormal.x + positionOnOriginNormal.y + positionOnOriginNormal.z));
                auto rotatedPosition = glm::abs(originNormal) * originPosition + (halfSize + distance) * faceNormal + glm::abs(rotationAxis) * position;
                
                auto distanceOnRotationAxisHigher = glm::abs(rotatedPosition - higher) * glm::abs(rotationAxis);
                auto distanceOnHigher = distanceOnRotationAxisHigher.x + distanceOnRotationAxisHigher.y + distanceOnRotationAxisHigher.z;
                auto rotatedPosition90 = higher + distance * rotationAxis + distanceOnHigher * faceNormal;
                
                auto distanceOnRotationAxisLower = glm::abs(rotatedPosition - lower) * glm::abs(rotationAxis);
                auto distanceOnLower = distanceOnRotationAxisLower.x + distanceOnRotationAxisLower.y + distanceOnRotationAxisLower.z;
                auto rotatedPositionMinus90 = lower - distance * rotationAxis + distanceOnLower * faceNormal;

                std::array<float, 3> distances;
                distances[0] = glm::distance(originPosition, rotatedPosition);
                distances[1] = glm::distance(originPosition, rotatedPosition90);
                distances[2] = glm::distance(originPosition, rotatedPositionMinus90);

                distanceDataX[i] = *std::min_element(begin(distances), end(distances));
            }
            else
            {
                std::array<glm::vec3, 5> originalPositions;
                std::array<glm::vec3, 5> rotatedPositions;
                originalPositions[0] = originPosition;
                
                glm::vec3 helperNormal {originNormal.y, originNormal.z, originNormal.x };            
                auto rotationAxis = glm::cross(originNormal, helperNormal);
                
                auto halfSize = 1.0;            

                auto originPositionOnHelperNormal = originPosition * glm::abs(helperNormal) - helperNormal;
                auto positionOnHelperNormal = position * glm::abs(helperNormal) - helperNormal;            
                
                auto ogDistance = glm::abs(originPositionOnHelperNormal.x + originPositionOnHelperNormal.y + originPositionOnHelperNormal.z);
                auto distance = glm::abs(positionOnHelperNormal.x + positionOnHelperNormal.y + positionOnHelperNormal.z);
                
                rotatedPositions[0] = glm::abs(originNormal) * originPosition + (3 * halfSize + distance) * helperNormal + glm::abs(rotationAxis) * position;
                
                auto originalHigher = originNormal + helperNormal + rotationAxis;
                auto higher = originNormal + 3 * halfSize * helperNormal + rotationAxis;
                
                for(char j = 0; j < 4; ++j)
                {
                    auto ogDistanceOnAxisHigher = glm::abs(originalPositions[j] - originalHigher) * glm::abs(rotationAxis);
                    auto ogDistanceOnHigher = ogDistanceOnAxisHigher.x + ogDistanceOnAxisHigher.y + ogDistanceOnAxisHigher.z;
                    originalPositions[j + 1] = originalHigher + ogDistance * rotationAxis - ogDistanceOnHigher * helperNormal;
                    originalHigher += 2 * halfSize * rotationAxis;
                    
                    auto originPositionOnHelperNormal = originalPositions[j + 1] * glm::abs(helperNormal) - helperNormal;
                    ogDistance = glm::abs(originPositionOnHelperNormal.x + originPositionOnHelperNormal.y + originPositionOnHelperNormal.z);
                                    
                    auto distanceOnRotationAxisHigher = glm::abs(rotatedPositions[j] - higher) * glm::abs(rotationAxis);
                    auto distanceOnHigher = distanceOnRotationAxisHigher.x + distanceOnRotationAxisHigher.y + distanceOnRotationAxisHigher.z;
                    rotatedPositions[j + 1] = higher + distance * rotationAxis + distanceOnHigher * helperNormal;
                    higher += 2 * halfSize * rotationAxis;
                    
                    auto positionOnHelperNormal = rotatedPositions[j + 1] * glm::abs(helperNormal) - 3.0 * halfSize * helperNormal;
                    distance = glm::abs(positionOnHelperNormal.x + positionOnHelperNormal.y + positionOnHelperNormal.z);
                }
                
                std::vector<float> distances;
                
                for(char j = 0; j < 5; ++j)
                {
                    if(j != 0)
                        distances.push_back(glm::distance(originalPositions[j], rotatedPositions[j - 1]));                    
                    
                    distances.push_back(glm::distance(originalPositions[j], rotatedPositions[j]));
                    
                    if(j != 4)
                        distances.push_back(glm::distance(originalPositions[j], rotatedPositions[j + 1]));             
                }
                
                distanceDataX[i] = *std::min_element(begin(distances), end(distances));
            }
            
            maskTextureDataX[i] = 255;
        }

    for(auto layer : normalLayers)
        deleteLayer(layer->name());
    
    for(auto layer : posLayers)
        deleteLayer(layer->name());
                
    if(activeTexels.size())
    {
        auto texelCount = seedLayer->width() * seedLayer->height();
        auto validateTexel = [&texelCount](const auto& texel) { return texel > 0 && texel < texelCount; };

        do
        {
            auto currentTexelIndex = activeTexels.cbegin()->second;
            activeTexels.erase(activeTexels.cbegin());
            
            for(int i = 0; i < 8; ++i)
            {
                int neighborTexelIndex = currentTexelIndex + neighborOffsets[i];

                if (validateTexel(neighborTexelIndex))
                {
                    int neighborhoodDataValue = neighborhoodData[2 * currentTexelIndex];

                    if (neighborhoodDataValue >= 0 || (-neighborhoodDataValue & (1 << i)) == 0)
                    {
                        int indirectIndex = 2 * neighborTexelIndex;
                        neighborTexelIndex = neighborhoodData[indirectIndex + 1] * seedTexture->width() + neighborhoodData[indirectIndex];

                        auto validTexel = validateTexel(neighborTexelIndex);
                        if (!validTexel || validTexel && neighborhoodData[2 * neighborTexelIndex] > 0)
                            neighborTexelIndex = 0;
                    }
                }
                else
                    neighborTexelIndex = 0;

                if(neighborTexelIndex != 0)
                {
                    float distanceToNeighbor = distanceData[currentTexelIndex];
                    
                    if(i < 4)
                        distanceToNeighbor += std::sqrt(areaData[currentTexelIndex])/2.0 + std::sqrt(areaData[neighborTexelIndex])/2.0;
                    else
                        distanceToNeighbor += std::sqrt(2 * areaData[currentTexelIndex])/2.0 + std::sqrt(2 * areaData[neighborTexelIndex])/2.0;
                        
                    if(distanceToNeighbor <= distance && distanceData[neighborTexelIndex] > distanceToNeighbor)
                    {
                        if(inspectedTexels[neighborTexelIndex] > 1)
                        {
                            activeTexels.insert({distanceToNeighbor, neighborTexelIndex});
                            inspectedTexels[neighborTexelIndex] = 1;
                        }
                        else if(inspectedTexels[neighborTexelIndex] == 1)
                        {
                            auto found = activeTexels.find({distanceData[neighborTexelIndex], neighborTexelIndex});
                            if(found != end(activeTexels))
                            {
                                activeTexels.erase(found);
                                activeTexels.insert({distanceToNeighbor, neighborTexelIndex});
                            }
                            
                        }
                        
                        if(neighborhoodData[2 * neighborTexelIndex] == 0 && neighborhoodData[2 * neighborTexelIndex + 1] == 0)
                            auto a = 0;
                        else
                            distanceData[neighborTexelIndex] = distanceToNeighbor;                
                    }
                }
            }
            
            inspectedTexels[currentTexelIndex] = 0;
            if(normalMask[currentTexelIndex])
                maskTextureData[currentTexelIndex] = 255;
        }while(!activeTexels.empty());
        
        for(auto& distance: distanceData)
            distance /= 100.0;
        
        auto distanceByteData = reinterpret_cast<glm::byte*>(distanceData.data());
        std::copy(distanceByteData, distanceByteData + distanceData.size() * sizeof(float), begin(valueTextureData));
        
        auto distanceLayer = mResourceManager->createLayer(layerName, Layer::Type::Float32, seedLayer->resolution(), valueTextureData, maskTextureData, seedLayer->palette());
        addActiveLayer(distanceLayer);

        distanceByteData = reinterpret_cast<glm::byte*>(distanceDataX.data());
        std::copy(distanceByteData, distanceByteData + distanceDataX.size() * sizeof(float), begin(valueTextureData));
                
        auto distanceXLayer = mResourceManager->createLayer("RealDistance", Layer::Type::Float32, seedLayer->resolution(), valueTextureData, maskTextureDataX, seedLayer->palette());
        addActiveLayer(distanceXLayer);
                
        std::vector<float> difference(areaData.size(), std::numeric_limits<float>::lowest());        
        float max = std::numeric_limits<float>::lowest();
        float min = std::numeric_limits<float>::max();
        float mean = 0;
        float finalDistance = 0;
        uint64_t elements = 0;
        
        for(uint64_t i = 0; i < distanceData.size(); i++)
            if(maskTextureDataX[i])
            {
                if(distanceDataX[i] > finalDistance)
                    finalDistance = distanceDataX[i]; 
                
                auto diff = abs(distanceDataX[i] - distanceData[i]);
                
                if(diff > 0 && diff < 10000000)
                {
                    if(diff < min)
                        min = diff;
                    else if (diff > max)
                        max = diff;
                    
                    mean += diff;
                    elements++;
                }
                
                difference[i] = diff;
            }
            
        mean /= elements;
        
        std::cout << std::setprecision(8) << "Precision Test::min: " << min << ", max: " << max << ", mean: " << mean << ", finalDistance: " << finalDistance << std::endl;
        
        distanceByteData = reinterpret_cast<glm::byte*>(difference.data());
        std::copy(distanceByteData, distanceByteData + difference.size() * sizeof(float), begin(valueTextureData));
                
        auto errorLayer = mResourceManager->createLayer("Error", Layer::Type::Float32, seedLayer->resolution(), valueTextureData, maskTextureDataX, seedLayer->palette());
        addActiveLayer(errorLayer);
                
        mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));

        mRenderer->padLayerTextures(0);
                
        return distanceLayer;
    }
    
    return nullptr;
}

Layer * Chisel::computeDistanceBandLayer(std::string layerName, unsigned int index, double distance)
{
    auto seedLayer = mActiveLayers[index];
    auto seedTexture = seedLayer->maskTexture();

    mRenderer->updateLayerSizeDependentEditingResources(seedLayer->resolution());
    mRenderer->updateAreaAndTopologyTechs(seedLayer->resolution());
    mGLWidget->repaint();

    auto areaTexture = mResourceManager->texture("AreaPerPixel");
    auto neighborhoodTexture = mResourceManager->texture("Neighborhood");

    auto seedData = mRenderer->readLayerMask(index);
    auto areaData = mRenderer->readFloatTexture(areaTexture);
    auto neighborhoodData = mRenderer->readFloatTexture(neighborhoodTexture);
    std::vector<float> distanceData(areaData.size(), std::numeric_limits<float>::max());

    std::vector<glm::byte> valueTextureData(distanceData.size() * sizeof(float), 0);
    std::vector<glm::byte> maskTextureData(distanceData.size(), 0);

    std::set<std::pair<double, uint64_t>> activeTexels;
    std::vector<char> inspectedTexels(areaData.size(), 2);
    //glm::ivec2 immediateOffsets[8] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}};    
    //std::array<int, 8> neighborOffsets = { -seedTexture->width(), seedTexture->width(), 1, -1, -seedTexture->width() + 1, -seedTexture->width() - 1, seedTexture->width() + 1, seedTexture->width() - 1 };
    std::array<int, 8> neighborOffsets = { -1, 1, seedTexture->width(), -seedTexture->width(), -1 + seedTexture->width(), -1 - seedTexture->width(), 1 + seedTexture->width(), 1 - seedTexture->width() };

    for (auto i = 0; i < seedData.size(); ++i)
        if (seedData[i] && neighborhoodData[2 * i] < 0)
        {
            activeTexels.insert({ 0,i });
            distanceData[i] = 0.0f;
            inspectedTexels[i] = 1;
        }

    if (activeTexels.size())
    {
        auto texelCount = seedLayer->width() * seedLayer->height();
        auto validateTexel = [&texelCount](const auto& texel) { return texel > 0 && texel < texelCount; };

        do
        {
            auto currentTexelIndex = activeTexels.cbegin()->second;
            activeTexels.erase(activeTexels.cbegin());

            for (int i = 0; i < 8; ++i)
            {
                int neighborTexelIndex = currentTexelIndex + neighborOffsets[i];

                if (validateTexel(neighborTexelIndex))
                {
                    int neighborhoodDataValue = neighborhoodData[2 * currentTexelIndex];

                    if (neighborhoodDataValue >= 0 || (-neighborhoodDataValue & (1 << i)) == 0)
                    {
                        int indirectIndex = 2 * neighborTexelIndex;
                        neighborTexelIndex = neighborhoodData[indirectIndex + 1] * seedTexture->width() + neighborhoodData[indirectIndex];

                        auto validTexel = validateTexel(neighborTexelIndex);
                        if (!validTexel || validTexel && neighborhoodData[2 * neighborTexelIndex] > 0)
                            neighborTexelIndex = 0;
                    }
                }
                else
                    neighborTexelIndex = 0;

                if (neighborTexelIndex != 0)
                {
                    float distanceToNeighbor = distanceData[currentTexelIndex];

                    if (i < 4)
                        distanceToNeighbor += std::sqrt(areaData[currentTexelIndex]) / 2.0 + std::sqrt(areaData[neighborTexelIndex]) / 2.0;
                    else
                        distanceToNeighbor += std::sqrt(2 * areaData[currentTexelIndex]) / 2.0 + std::sqrt(2 * areaData[neighborTexelIndex]) / 2.0;

                    if (distanceData[neighborTexelIndex] > distanceToNeighbor)
                    {
                        if (inspectedTexels[neighborTexelIndex] > 1)
                        {
                            activeTexels.insert({ distanceToNeighbor, neighborTexelIndex });
                            inspectedTexels[neighborTexelIndex] = 1;
                        }
                        else if (inspectedTexels[neighborTexelIndex] == 1)
                        {
                            auto found = activeTexels.find({ distanceData[neighborTexelIndex], neighborTexelIndex });
                            if (found != end(activeTexels))
                            {
                                activeTexels.erase(found);
                                activeTexels.insert({ distanceToNeighbor, neighborTexelIndex });
                            }

                        }

                        if (neighborhoodData[2 * neighborTexelIndex] == 0 && neighborhoodData[2 * neighborTexelIndex + 1] == 0)
                            auto a = 0;
                        else
                            distanceData[neighborTexelIndex] = distanceToNeighbor;
                    }
                }
            }

            inspectedTexels[currentTexelIndex] = 0;
            maskTextureData[currentTexelIndex] = 255;

            //         LOG("Active texels size: ", activeTexels.size());
        } while (!activeTexels.empty());

        for (auto& texel : distanceData)
            if (texel > 0)
                texel = (texel <= distance) ? 1 : 2;

        auto distanceByteData = reinterpret_cast<glm::byte*>(distanceData.data());
        std::copy(distanceByteData, distanceByteData + distanceData.size() * sizeof(float), begin(valueTextureData));

        auto distanceLayer = mResourceManager->createLayer(layerName, Layer::Type::Float32, seedLayer->resolution(), valueTextureData, maskTextureData, seedLayer->palette());
        addActiveLayer(distanceLayer);

        mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));

        mRenderer->padLayerTextures(0);

        return distanceLayer;
    }

    return nullptr;
}

Layer* Chisel::computeCurvatureLayer(std::string layerName, const std::pair<int, int> layerResolution, double distance)
{
    mRenderer->updateLayerSizeDependentEditingResources(layerResolution);
    mRenderer->updateAreaAndTopologyTechs(layerResolution);
    mGLWidget->repaint();

    auto normalLayers = computeNormalLayer("Normals", layerResolution);
    
    auto neighborhoodTexture = mResourceManager->texture("Neighborhood");    
    auto neighborhoodData = mRenderer->readFloatTexture(neighborhoodTexture);
    
    std::array<std::vector<float>, 3> normalData;
    normalData[0] = mRenderer->readFloatTexture(normalLayers[0]->dataTexture());
    normalData[1] = mRenderer->readFloatTexture(normalLayers[1]->dataTexture());
    normalData[2] = mRenderer->readFloatTexture(normalLayers[2]->dataTexture());
    auto normalMask = mRenderer->readTexture(normalLayers[0]->maskTexture());

    auto width = normalLayers[0]->dataTexture()->width();
    auto height = normalLayers[0]->dataTexture()->height();
    auto texelCount = width * height;
    
    auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
    auto maxArea = *std::max_element(begin(areaData), end(areaData));
    
    std::vector<float> curvatureData(normalMask.size(), 0);
    std::vector<glm::byte> valueTextureData(normalMask.size() * sizeof(float), 0);
    std::vector<glm::byte> maskTextureData(normalMask.size(), 0);

    std::array<int, 8> neighborOffsets = { -1, 1, width, -width, -1 + width, -1 - width, 1 + width, 1 - width };
    
    std::vector<uint64_t> texelCostData(normalMask.size(), std::numeric_limits<uint64_t>::max());
    std::vector<double> metricCostData(normalMask.size(), std::numeric_limits<double>::max());
    
    std::vector<char> inspectedTexels(normalMask.size(), 2);
    
    for(auto i = 0; i < normalMask.size(); ++i)
        if(normalMask[i] && neighborhoodData[2 * i] < 0)
        {
            std::vector<uint64_t> validIndices;
            std::vector<glm::vec3> normals;
                       
            std::vector<uint64_t> dirtyIndices;
            std::set<std::pair<uint64_t, uint64_t>> activeTexels;
            
            activeTexels.insert({0,i});
            texelCostData[i] = 0;            
            metricCostData[i] = 0;
            inspectedTexels[i] = 1;
                        
            do
            {
                auto currentTexelIndex = activeTexels.cbegin()->second;
                activeTexels.erase(activeTexels.cbegin());
                
                if(normalMask[currentTexelIndex])
                {
                    /*if(surfaceCostData[currentTexelIndex] < radius)     */               
                        for(int idx = 0; idx < 8; ++idx)
                        {
                            int neighborTexelIndex = currentTexelIndex + neighborOffsets[idx];

                            if (neighborTexelIndex > 0 && neighborTexelIndex < texelCount)
                            {
                                int neighborhoodDataValue = neighborhoodData[2 * currentTexelIndex];

                                if (neighborhoodDataValue >= 0 || (-neighborhoodDataValue & (1 << idx)) == 0)
                                {
                                    int indirectIndex = 2 * neighborTexelIndex;
                                    neighborTexelIndex = neighborhoodData[indirectIndex + 1] * width + neighborhoodData[indirectIndex];

                                    auto validTexel = neighborTexelIndex > 0 && neighborTexelIndex < texelCount;
                                    if (!validTexel || validTexel && neighborhoodData[2 * neighborTexelIndex] > 0)
                                        neighborTexelIndex = 0;
                                }
                            }
                            else
                                neighborTexelIndex = 0;

                            if (neighborTexelIndex != 0)
                            {
                                float texelCostToNeighbor = texelCostData[currentTexelIndex] + 1;                                
                                float metricCostToNeighbor = metricCostData[currentTexelIndex];
                                
                                if(i < 4)
                                    metricCostToNeighbor += std::sqrt(areaData[currentTexelIndex])/2.0 + std::sqrt(areaData[neighborTexelIndex])/2.0;
                                else
                                    metricCostToNeighbor += std::sqrt(2 * areaData[currentTexelIndex])/2.0 + std::sqrt(2 * areaData[neighborTexelIndex])/2.0;                                
                                                            
                                if(texelCostToNeighbor <= distance && texelCostData[neighborTexelIndex] > texelCostToNeighbor)
                                {
                                    if(inspectedTexels[neighborTexelIndex] > 1)
                                    {
                                        activeTexels.insert({texelCostToNeighbor, neighborTexelIndex});
                                        inspectedTexels[neighborTexelIndex] = 1;
                                    }
                                    else if(inspectedTexels[neighborTexelIndex] == 1)
                                    {
                                        auto found = activeTexels.find({texelCostData[neighborTexelIndex], neighborTexelIndex});
                                        if(found != end(activeTexels))
                                        {
                                            activeTexels.erase(found);
                                            activeTexels.insert({texelCostToNeighbor, neighborTexelIndex});
                                        }                                
                                    }
                                    
                                    if(neighborhoodData[2 * neighborTexelIndex] == 0 && neighborhoodData[2 * neighborTexelIndex + 1] == 0)
                                        auto a = 0;
                                    else
                                    {
                                        texelCostData[neighborTexelIndex] = texelCostToNeighbor;
                                        metricCostData[neighborTexelIndex] = metricCostToNeighbor;
                                        dirtyIndices.push_back(neighborTexelIndex);
                                    }
                                }
                            }
                        }
                    
                    inspectedTexels[currentTexelIndex] = 0;
                    
                    normals.push_back({normalData[0][currentTexelIndex], normalData[1][currentTexelIndex], normalData[2][currentTexelIndex]});
                    validIndices.push_back(currentTexelIndex);

                    dirtyIndices.push_back(currentTexelIndex);
                }
            }while(!activeTexels.empty());
            
            std::vector<double> radii;
            std::vector<double> weights;             
            
            for(auto j = 1; j < normals.size(); ++j)
            {
                double angle = std::atan2(glm::dot(normals[j], glm::normalize(glm::cross(normals[0], glm::cross(normals[0], normals[j])))), glm::dot(normals[j], normals[0]));
                
                auto neighborIndex = validIndices[j];
                
                radii.push_back(metricCostData[neighborIndex] * 0.5 / std::fabs(std::sin(angle * 0.5)));
                weights.push_back(areaData[neighborIndex] / maxArea * 1.0/metricCostData[neighborIndex]);
            }
                        
            double radiiSum = 0;
            double weightsSum = 0;
            
            for(auto j = 0; j < radii.size(); ++j)
                if(!std::isnan(radii[j]))
                {
                    radiiSum += weights[j] * radii[j];
                    weightsSum += weights[j];                    
                }
                
            curvatureData[i] = 1.0/(radiiSum/weightsSum);
            maskTextureData[i] = 255;

            for(const auto& index: dirtyIndices)
            {
                texelCostData[index] = std::numeric_limits<uint64_t>::max();
                metricCostData[index] = std::numeric_limits<double>::max();
                inspectedTexels[index] = 2;
            }            
// 		for(NodeIndex j = 0; j < neighbourIndexesGlobalList.size(); ++j)
// 		{
// 				NodeIndex neighbourIndex = neighbourIndexesGlobalList[j];
// 				double angle = atan2( normalsAtCurrentRes[neighbourIndex] * ((normalsAtCurrentRes[neighbourIndex].CrossProduct(normalsAtCurrentRes[i])).CrossProduct(normalsAtCurrentRes[i])).normalize(), normalsAtCurrentRes[neighbourIndex] * normalsAtCurrentRes[i] );
// 
// 				double distance = metricDistance[j] * 0.5;
// 
// 				radiusList.push_back(distance / fabs(sin(angle * 0.5)));
// 				weightList.push_back(areaVoxelLayer->getDataAt(j)/ max * 1.0 / distance);
// 		}
// 
// 		double radiusMean = 0;
// 		double weightMean = 0;
// 
// 		for(int j = 0; j < radiusList.size(); ++j)
// 			if( !chisel::utilities::GlobalFunctions::isNan(radiusList[j]) )
// 			{
// 				radiusMean += weightList[j] * radiusList[j];
// 				weightMean += weightList[j];
// 			}            
        }
    
    for(auto layer : normalLayers)
        deleteLayer(layer->name());
        //mResourceManager->deleteLayer(layer);    
    
    auto curvatureByteData = reinterpret_cast<glm::byte*>(curvatureData.data());
    std::copy(curvatureByteData, curvatureByteData + curvatureData.size() * sizeof(float), begin(valueTextureData));

    auto curvatureLayer = mResourceManager->createLayer(layerName, Layer::Type::Float32, layerResolution, valueTextureData, maskTextureData, mResourceManager->palette(0));
    addActiveLayer(curvatureLayer);
    setCurrentLayer(0);
    
    mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));
    mRenderer->padLayerTextures(0); 
    
    return curvatureLayer;
}

Layer* Chisel::computeRoughnessLayer(std::string layerName, const std::pair<int, int> layerResolution, double distance)
{
    mRenderer->updateLayerSizeDependentEditingResources(layerResolution);
    mRenderer->updateAreaAndTopologyTechs(layerResolution);
    mGLWidget->repaint();

    auto normalLayers = computeNormalLayer("Normals", layerResolution);    
    auto posLayers = computePositionLayer("Position", layerResolution);
    
    auto neighborhoodTexture = mResourceManager->texture("Neighborhood");    
    auto neighborhoodData = mRenderer->readFloatTexture(neighborhoodTexture);
    
    std::array<std::vector<float>, 3> normalData;
    normalData[0] = mRenderer->readFloatTexture(normalLayers[0]->dataTexture());
    normalData[1] = mRenderer->readFloatTexture(normalLayers[1]->dataTexture());
    normalData[2] = mRenderer->readFloatTexture(normalLayers[2]->dataTexture());
    auto normalMask = mRenderer->readTexture(normalLayers[0]->maskTexture());
    
    std::array<std::vector<float>, 3> posData;
    posData[0] = mRenderer->readFloatTexture(posLayers[0]->dataTexture());
    posData[1] = mRenderer->readFloatTexture(posLayers[1]->dataTexture());
    posData[2] = mRenderer->readFloatTexture(posLayers[2]->dataTexture());
        
    auto width = normalLayers[0]->dataTexture()->width();
    auto height = normalLayers[0]->dataTexture()->height();
    auto texelCount = width * height;
    
    auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
    auto maxArea = *std::max_element(begin(areaData), end(areaData));
    
    std::vector<float> roughnessData(normalMask.size(), 0);
    std::vector<glm::byte> valueTextureData(normalMask.size() * sizeof(float), 0);
    std::vector<glm::byte> maskTextureData(normalMask.size(), 0);

    std::array<int, 8> neighborOffsets = { -1, 1, width, -width, -1 + width, -1 - width, 1 + width, 1 - width };
    
    std::vector<uint64_t> texelCostData(normalMask.size(), std::numeric_limits<uint64_t>::max());
    std::vector<double> metricCostData(normalMask.size(), std::numeric_limits<double>::max());
    
    std::vector<char> inspectedTexels(normalMask.size(), 2);
    
    for(auto i = 0; i < normalMask.size(); ++i)
        if(normalMask[i] && neighborhoodData[2 * i] < 0)
        {
            std::vector<uint64_t> validIndices;
            std::vector<glm::vec3> normals;
                       
            std::vector<uint64_t> dirtyIndices;
            std::set<std::pair<uint64_t, uint64_t>> activeTexels;
            
            activeTexels.insert({0,i});
            texelCostData[i] = 0;            
            metricCostData[i] = 0;
            inspectedTexels[i] = 1;
                        
            do
            {
                auto currentTexelIndex = activeTexels.cbegin()->second;
                activeTexels.erase(activeTexels.cbegin());
                
                if(normalMask[currentTexelIndex])
                {
                    /*if(surfaceCostData[currentTexelIndex] < radius)     */               
                        for(int idx = 0; idx < 8; ++idx)
                        {
                            int neighborTexelIndex = currentTexelIndex + neighborOffsets[idx];

                            if (neighborTexelIndex > 0 && neighborTexelIndex < texelCount)
                            {
                                int neighborhoodDataValue = neighborhoodData[2 * currentTexelIndex];

                                if (neighborhoodDataValue >= 0 || (-neighborhoodDataValue & (1 << idx)) == 0)
                                {
                                    int indirectIndex = 2 * neighborTexelIndex;
                                    neighborTexelIndex = neighborhoodData[indirectIndex + 1] * width + neighborhoodData[indirectIndex];

                                    auto validTexel = neighborTexelIndex > 0 && neighborTexelIndex < texelCount;
                                    if (!validTexel || validTexel && neighborhoodData[2 * neighborTexelIndex] > 0)
                                        neighborTexelIndex = 0;
                                }
                            }
                            else
                                neighborTexelIndex = 0;

                            if (neighborTexelIndex != 0)
                            {
                                float texelCostToNeighbor = texelCostData[currentTexelIndex] + 1;                                
                                float metricCostToNeighbor = metricCostData[currentTexelIndex];
                                
                                if(i < 4)
                                    metricCostToNeighbor += std::sqrt(areaData[currentTexelIndex])/2.0 + std::sqrt(areaData[neighborTexelIndex])/2.0;
                                else
                                    metricCostToNeighbor += std::sqrt(2 * areaData[currentTexelIndex])/2.0 + std::sqrt(2 * areaData[neighborTexelIndex])/2.0;                                
                                                            
                                if(texelCostToNeighbor <= distance && texelCostData[neighborTexelIndex] > texelCostToNeighbor)
                                {
                                    if(inspectedTexels[neighborTexelIndex] > 1)
                                    {
                                        activeTexels.insert({texelCostToNeighbor, neighborTexelIndex});
                                        inspectedTexels[neighborTexelIndex] = 1;
                                    }
                                    else if(inspectedTexels[neighborTexelIndex] == 1)
                                    {
                                        auto found = activeTexels.find({texelCostData[neighborTexelIndex], neighborTexelIndex});
                                        if(found != end(activeTexels))
                                        {
                                            activeTexels.erase(found);
                                            activeTexels.insert({texelCostToNeighbor, neighborTexelIndex});
                                        }                                
                                    }
                                    
                                    if(neighborhoodData[2 * neighborTexelIndex] == 0 && neighborhoodData[2 * neighborTexelIndex + 1] == 0)
                                        auto a = 0;
                                    else
                                    {
                                        texelCostData[neighborTexelIndex] = texelCostToNeighbor;
                                        metricCostData[neighborTexelIndex] = metricCostToNeighbor;
                                        dirtyIndices.push_back(neighborTexelIndex);
                                    }
                                }
                            }
                        }
                    
                    inspectedTexels[currentTexelIndex] = 0;
                    
                    normals.push_back({normalData[0][currentTexelIndex], normalData[1][currentTexelIndex], normalData[2][currentTexelIndex]});
                    validIndices.push_back(currentTexelIndex);

                    dirtyIndices.push_back(currentTexelIndex);
                }
            }while(!activeTexels.empty());
            
            std::vector<double> radii;
            std::vector<double> weights;             
            
            for(auto j = 1; j < normals.size(); ++j)
            {
                double angle = std::atan2(glm::dot(normals[j], glm::normalize(glm::cross(normals[0], glm::cross(normals[0], normals[j])))), glm::dot(normals[j], normals[0]));
                
                if(std::fabs(angle) > 1e-8)
                {
                    auto neighborIndex = validIndices[j];
                    
                    radii.push_back(metricCostData[neighborIndex] * 0.5 / std::fabs(std::sin(angle * 0.5)));
                    weights.push_back(areaData[neighborIndex] / maxArea * 1.0/metricCostData[neighborIndex]);
                }
            }
                        
            double radiusMean = 0;
            
            for(auto j = 0; j < radii.size(); ++j)
                if(!std::isnan(radii[j]))
                    radiusMean += radii[j];
            
            if(radiusMean) radiusMean /= radii.size();
            
            auto center = glm::dvec3{posData[0][i], posData[1][i], posData[2][i]} - radiusMean * glm::dvec3(normals[0]);

            std::vector<double> diffs;
            
            for(auto j = 1; j < normals.size(); ++j)
            {
                auto neighborIndex = validIndices[j];                
                glm::dvec3 neighPos{posData[0][neighborIndex], posData[1][neighborIndex], posData[2][neighborIndex]};
                auto distCenterToNeighbor = glm::distance(center, neighPos);
                
                diffs.push_back(std::fabs(radiusMean - distCenterToNeighbor));
            }
            
            double diffMean = 0;
            
            for(auto diff : diffs)
                diffMean += diff;
            
            roughnessData[i] = (!std::isnan(diffMean)) ? diffMean / diffs.size() : 0;                
            maskTextureData[i] = 255;

            for(const auto& index: dirtyIndices)
            {
                texelCostData[index] = std::numeric_limits<uint64_t>::max();
                metricCostData[index] = std::numeric_limits<double>::max();
                inspectedTexels[index] = 2;
            }
        }

    for(auto layer : normalLayers)
        deleteLayer(layer->name());
    
    for(auto layer : posLayers)
        deleteLayer(layer->name());
        
    auto roughnessByteData = reinterpret_cast<glm::byte*>(roughnessData.data());
    std::copy(roughnessByteData, roughnessByteData + roughnessData.size() * sizeof(float), begin(valueTextureData));

    auto roughnessLayer = mResourceManager->createLayer(layerName, Layer::Type::Float32, layerResolution, valueTextureData, maskTextureData, mResourceManager->palette(0));
    addActiveLayer(roughnessLayer);

    mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));
    mRenderer->padLayerTextures(0); 
    
    return roughnessLayer;
}

std::array<Layer*, 3> Chisel::computePositionLayer(std::string layerName, const std::pair<int, int> layerResolution)
{
    auto posLayerX = createLayer(mResourceManager->createValidLayerName(layerName + "X"), Layer::Type::Float32, layerResolution);
    auto posLayerY = createLayer(mResourceManager->createValidLayerName(layerName + "Y"), Layer::Type::Float32, layerResolution);
    auto posLayerZ = createLayer(mResourceManager->createValidLayerName(layerName + "Z"), Layer::Type::Float32, layerResolution);
    
    mRenderer->computeLayerOperation(2);
    mRenderer->padLayerTextures(0);

    return {posLayerX, posLayerY, posLayerZ};
}

std::array<Layer*, 3> Chisel::computeNormalLayer(std::string layerName, const std::pair<int, int> layerResolution)
{
    auto normalLayerX = createLayer(mResourceManager->createValidLayerName(layerName + "X"), Layer::Type::Float32, layerResolution);
    auto normalLayerY = createLayer(mResourceManager->createValidLayerName(layerName + "Y"), Layer::Type::Float32, layerResolution);
    auto normalLayerZ = createLayer(mResourceManager->createValidLayerName(layerName + "Z"), Layer::Type::Float32, layerResolution);
    
    mRenderer->computeLayerOperation(0);
    mRenderer->padLayerTextures(0);
    
    return {normalLayerX, normalLayerY, normalLayerZ};
}

Layer* Chisel::computeOrientationLayer(std::string layerName, const std::pair<int, int> layerResolution, glm::vec3 reference)
{   
    auto orientationLayer = createLayer(layerName, Layer::Type::Float32, layerResolution);
    
    std::vector<glm::byte> uniformData(4 * sizeof(float), 0);    
    auto referenceDataPtr = reinterpret_cast<glm::byte*>(glm::value_ptr(reference));    
    std::copy(referenceDataPtr, referenceDataPtr + 3 * sizeof(float), begin(uniformData));
    
    mRenderer->computeLayerOperation(1, uniformData);
    mRenderer->padLayerTextures(0);
    
    return orientationLayer;
}

Layer * Chisel::computeResampling(std::string layerName, unsigned int sourceLayerIndex, unsigned int fieldIndex, std::pair<int, int> resolution, StatOps operation)
{
    auto sourceLayer = mActiveLayers[sourceLayerIndex];
    std::vector<double> sourceData;

    auto resampledLayer = createLayer(layerName, sourceLayer->type(), resolution);

    //if (sourceLayer->registerType() && fieldIndex > -1)
    //{
    //    auto regLayer = static_cast<RegisterLayer*>(sourceLayer);
    //    auto fieldLayer = mResourceManager->createLayerFromTableField(regLayer, regLayer->field(fieldIndex));
    //    mMainWindow->visualizer()->update(); 
    //    sourceData = mRenderer->readLayerDataTexture(fieldLayer->dataTexture());
    //    mResourceManager->deleteLayer(fieldLayer);
    //}
    //else
    //    sourceData = mRenderer->readLayerData(sourceLayerIndex);

    mRenderer->resampleLayer(sourceLayerIndex + 1, 0);

    //std::vector<glm::byte> uniformData(sizeof(glm::uvec4), 0);
    //glm::uvec4 layerIndices = glm::uvec4(sourceLayer->dataTexture()->textureArrayIndices(), sourceLayer->maskTexture()->textureArrayIndices());
    //auto layerIndicesPtr = reinterpret_cast<glm::byte*>(glm::value_ptr(layerIndices));
    //std::copy(layerIndicesPtr, layerIndicesPtr + sizeof(glm::uvec4), begin(uniformData));

    //mRenderer->computeLayerOperation(4, uniformData);

    //auto sourceMask = mRenderer->readLayerMask(sourceLayerIndex);
    //float radius = sourceLayer->width() / resampledLayer->width();

    //auto neighborhoodTexture = mResourceManager->texture("Neighborhood");
    //auto neighborhoodData = mRenderer->readFloatTexture(neighborhoodTexture);

    //std::vector<float> neighborhoodStatsData(sourceData.size(), 0);
    //std::vector<glm::byte> valueTextureData(sourceData.size() / radius, 0);
    //std::vector<glm::byte> maskTextureData(sourceData.size(), 0);

    //auto width = sourceLayer->dataTexture()->width();
    //auto height = sourceLayer->dataTexture()->height();
    //auto texelCount = width * height;
    //std::array<int, 8> neighborOffsets = { -1, 1, width, -width, -1 + width, -1 - width, 1 + width, 1 - width };

    //std::vector<uint64_t> surfaceCostData(sourceData.size(), std::numeric_limits<uint64_t>::max());
    //std::vector<char> inspectedTexels(sourceData.size(), 2);

    //for (auto i = 0; i < sourceMask.size(); ++i)
    //    if (sourceMask[i] && neighborhoodData[2 * i] < 0)
    //    {
    //        std::vector<double> statisticsData;
    //        std::vector<uint64_t> dirtyIndices;
    //        std::set<std::pair<uint64_t, uint64_t>> activeTexels;

    //        activeTexels.insert({ 0,i });
    //        surfaceCostData[i] = 0;
    //        inspectedTexels[i] = 1;

    //        do
    //        {
    //            auto currentTexelIndex = activeTexels.cbegin()->second;
    //            activeTexels.erase(activeTexels.cbegin());

    //            if (sourceMask[currentTexelIndex])
    //            {
    //                /*if(surfaceCostData[currentTexelIndex] < radius)     */
    //                for (int idx = 0; idx < 8; ++idx)
    //                {
    //                    int neighborTexelIndex = currentTexelIndex + neighborOffsets[idx];

    //                    if (neighborTexelIndex > 0 && neighborTexelIndex < texelCount)
    //                    {
    //                        int neighborhoodDataValue = neighborhoodData[2 * currentTexelIndex];

    //                        if (neighborhoodDataValue >= 0 || (-neighborhoodDataValue & (1 << idx)) == 0)
    //                        {
    //                            int indirectIndex = 2 * neighborTexelIndex;
    //                            neighborTexelIndex = neighborhoodData[indirectIndex + 1] * width + neighborhoodData[indirectIndex];

    //                            auto validTexel = neighborTexelIndex > 0 && neighborTexelIndex < texelCount;
    //                            if (!validTexel || validTexel && neighborhoodData[2 * neighborTexelIndex] > 0)
    //                                neighborTexelIndex = 0;
    //                        }
    //                    }
    //                    else
    //                        neighborTexelIndex = 0;

    //                    if (neighborTexelIndex != 0)
    //                    {
    //                        float costToNeighbor = surfaceCostData[currentTexelIndex] + 1;

    //                        if (costToNeighbor <= radius && surfaceCostData[neighborTexelIndex] > costToNeighbor)
    //                        {
    //                            if (inspectedTexels[neighborTexelIndex] > 1)
    //                            {
    //                                activeTexels.insert({ costToNeighbor, neighborTexelIndex });
    //                                inspectedTexels[neighborTexelIndex] = 1;
    //                            }
    //                            else if (inspectedTexels[neighborTexelIndex] == 1)
    //                            {
    //                                auto found = activeTexels.find({ surfaceCostData[neighborTexelIndex], neighborTexelIndex });
    //                                if (found != end(activeTexels))
    //                                {
    //                                    activeTexels.erase(found);
    //                                    activeTexels.insert({ costToNeighbor, neighborTexelIndex });
    //                                }
    //                            }

    //                            if (neighborhoodData[2 * neighborTexelIndex] == 0 && neighborhoodData[2 * neighborTexelIndex + 1] == 0)
    //                                auto a = 0;
    //                            else
    //                            {
    //                                surfaceCostData[neighborTexelIndex] = costToNeighbor;
    //                                dirtyIndices.push_back(neighborTexelIndex);
    //                            }
    //                        }
    //                    }
    //                }

    //                inspectedTexels[currentTexelIndex] = 0;
    //                statisticsData.push_back(sourceData[currentTexelIndex]);
    //                dirtyIndices.push_back(currentTexelIndex);
    //            }
    //        } while (!activeTexels.empty());

    //        for (const auto& index : dirtyIndices)
    //        {
    //            surfaceCostData[index] = std::numeric_limits<uint64_t>::max();
    //            inspectedTexels[index] = 2;
    //        }

    //        maskTextureData[i] = 255;

    //        switch (operation)
    //        {
    //        case StatOps::MeanValue:
    //        {
    //            neighborhoodStatsData[i] = std::accumulate(begin(statisticsData), end(statisticsData), 0.0) / statisticsData.size();
    //            auto b = neighborhoodStatsData[i];
    //            auto c = 0;
    //        }
    //        break;
    //        case StatOps::MinValue:
    //        {
    //            neighborhoodStatsData[i] = *std::min_element(begin(statisticsData), end(statisticsData));
    //        }
    //        break;
    //        case StatOps::MaxValue:
    //        {
    //            neighborhoodStatsData[i] = *std::max_element(begin(statisticsData), end(statisticsData));
    //        }
    //        break;
    //        case StatOps::Variance:
    //        {
    //            auto mean = std::accumulate(begin(statisticsData), end(statisticsData), 0.0) / statisticsData.size();

    //            auto const add_square = [&mean](double sum, double i)
    //            {
    //                auto d = i - mean;
    //                return sum + d * d;
    //            };

    //            neighborhoodStatsData[i] = std::accumulate(begin(statisticsData), end(statisticsData), 0.0, add_square) / (statisticsData.size() - 1);
    //        }
    //        break;
    //        case StatOps::StdDeviation:
    //        {
    //            auto mean = std::accumulate(begin(statisticsData), end(statisticsData), 0.0) / statisticsData.size();

    //            auto const add_square = [&mean](double sum, double i)
    //            {
    //                auto d = i - mean;
    //                return sum + d * d;
    //            };

    //            auto variance = std::accumulate(begin(statisticsData), end(statisticsData), 0.0, add_square) / (statisticsData.size() - 1);

    //            neighborhoodStatsData[i] = std::sqrt(variance);
    //        }
    //        break;
    //        case StatOps::NoNull:
    //        {
    //            //auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));

    //        }
    //        break;
    //        case StatOps::Null:
    //        {
    //            //auto areaData = mRenderer->readFloatTexture(mResourceManager->texture("AreaPerPixel"));
    //        }
    //        break;
    //        }
    //    }

    //auto statsByteData = reinterpret_cast<glm::byte*>(neighborhoodStatsData.data());
    //std::copy(statsByteData, statsByteData + valueTextureData.size(), begin(valueTextureData));



    return resampledLayer;
}

void Chisel::createPalette(std::string name)
{
    auto newPalette = mResourceManager->createPalette("new", Palette::Type::Discrete, {{0, {1.0, 1.0, 1.0, 1.0}},{ 1, {0.0, 0.0, 0.0, 1.0}}});
    mNewPaletteModel->setPalette(newPalette);
    mPaletteListModel->addPalette(newPalette);
}

void Chisel::editPalette(unsigned int index)
{
    mNewPaletteModel->setPalette(mResourceManager->palette(index));
}

void Chisel::importPalette(std::string name, std::string path)
{
    auto imported = mResourceManager->loadPalette(name, path);
    mPaletteListModel->addPalette(imported);
}

void Chisel::importPaletteToLayer(std::string name, std::string path)
{
    auto imported = mResourceManager->loadPalette(name, path, true);
    mCurrentLayer->copyPalette(*imported);
    mResourceManager->deleteLayerPalette(imported);
            
    mDirtyFlag = true;    
}

void Chisel::exportPalette(unsigned int index, std::string name, std::string path)
{
    mResourceManager->savePalette(index, name, path);
}

void Chisel::duplicatePalette(unsigned int index, bool validateName)
{    
    auto duplicated = mResourceManager->duplicatePalette(mResourceManager->palette(index), validateName);
    mPaletteListModel->addPalette(duplicated);
    mDirtyFlag = true;
}

void Chisel::deletePalette(unsigned int index)
{
    mResourceManager->deletePalette(index);
    mPaletteListModel->delPalette(index);  
}

void Chisel::deleteLastPalette()
{
    mResourceManager->deleteLastPalette();    
    mPaletteListModel->delPalette(mPaletteListModel->rowCount() - 1);
}

void Chisel::savePalette(unsigned int index)
{
    mResourceManager->savePalette(index, "", "palettes/");
}

void Chisel::copyPalette(unsigned int sourceIndex, unsigned int destinationIndex)
{
    auto source = mResourceManager->palette(sourceIndex);
    auto destination = mResourceManager->palette(destinationIndex);
    
    if(source->name() != destination->name())
        mResourceManager->renamePaletteFile(destinationIndex, source->name());
    
    destination->setName(source->name());
    destination->setControlPoints(source->controlPoints());
    destination->setInterpolation(source->isInterpolating());
    //mResourceManager->renamePalette(index, newName);
}


void Chisel::addPalette(Palette* palette)
{
    auto palettes = mPaletteListModel->palettes();
    
    auto search = std::find_if(begin(palettes), end(palettes), [&](const Palette* currentPalette){ return (palette->name() == currentPalette->name()) ? true : false;});
    
    if(search == end(palettes))
        mPaletteListModel->addPalette(palette);    
}

void Chisel::addPaletteToCollection()
{
    auto palette = mResourceManager->copyPaletteToCollection(mCurrentPalette);
    mPaletteListModel->addPalette(palette);    
}


void Chisel::eraseTempPalette(Palette* palette)
{
    if(!mResourceManager->paletteFileExits(palette->name()))
    {
        auto palettes = mPaletteListModel->localPalettes();
        int index = palette->index() - static_cast<int>(mPaletteListModel->palettes().size());
        
        if(index >= 0)
        {
            mResourceManager->deleteTempPalette(palette->index());
            for(int i = index + 1; i < palettes.size(); ++i)
                palettes[i]->setIndex(palettes[i]->index());
            mPaletteListModel->deleteLocalPalette(index);
        }
    }
}


void Chisel::addLocalPalette(Palette* palette)
{
    auto palettes = mPaletteListModel->localPalettes();
    
    auto search = std::find_if(begin(palettes), end(palettes), [&](const Palette* currentPalette){ return (palette == currentPalette) ? true : false;});
    
    if(search == end(palettes))
    {
//         mResourceManager->changePaletteToPermanent(palette);
        mPaletteListModel->addLocalPalette(palette);
/*        
        if(mCurrentLayer != nullptr)
        {
            mCurrentLayer->setDirty(true);
            mDirtyFlag = true;
        }*/
    }        
}

void Chisel::addLocalPalettes(const std::vector<Palette *>& palettes)
{
    
}

void Chisel::setCurrentPalette(unsigned int index)
{
    auto newPalette = mResourceManager->palette(index);
    
    if(newPalette != nullptr)
    {
        mCurrentPalette = newPalette;
                
        if(mCurrentLayer != nullptr)
        {
            mCurrentLayer->copyPalette(*mCurrentPalette);
            mCurrentPalette = mCurrentLayer->palette();
            //mCurrentLayer->setPalette(newPalette);
            //mResourceManager->createPaletteTexture(newPalette);
            //mResourceManager->createPaletteTexture(mCurrentLayer->name(), mCurrentLayer->palette());
            mRenderer->setCurrentPaletteTexture(mCurrentLayer->paletteTexture());
            updatePaletteTexture(); 
            
            mDirtyFlag = true;
        }
        
        mPaletteModel->setPalette(mCurrentPalette);
    }
    
}


void Chisel::setCurrentPalette(Palette* palette)
{
    mCurrentPalette = palette;
    
    //TODO Update the painting data
}

void Chisel::setCurrentPaintingValue(double value)
{
    //mCurrentPaintingData = {value, color};    
    mPaintingColorNeedUpdate = true;
    mPaintNewColor = true;
    
    mRenderer->setCurrentPaintingValue(value);
}

void Chisel::setPickedValue(double value)
{
    mMainWindow->setPickedValue(value);
}

void Chisel::setOpacity(unsigned int layer, float opacity)
{
    mActiveLayers[layer]->setOpacity(opacity);
    mRenderer->setOpacity(layer, opacity);
}

void Chisel::toggleVisibility(unsigned int layer)
{       
    auto selectedlayer = mActiveLayers[layer];
    selectedlayer->toggleVisibility();
    mRenderer->setOpacity(layer, (selectedlayer->isVisible()) ? selectedlayer->opacity() : 0.0f);
    
    mActiveLayerModel->setVisibililty(layer, selectedlayer->isVisible());
}

void Chisel::updateVisibleColumns()
{
    auto primary = mVisibleColumnModel->primaryColumn();
    RegisterLayer* layer = static_cast<RegisterLayer*>(mCurrentLayer);

    if(mDataBaseTableDataModel->isDirty())
    {
        mDataBaseTableDataModel->commitData();
        mCurrentRowDataModel->loadTable();
        mPickedRowDataModel->loadTable();
    }
    
    if(primary != layer->primaryColumn())
    {
        layer->setPrimaryColumn(primary);
        
        if(primary == 0 && mTempFieldLayers[mCurrentLayerIndex] != nullptr)
        {            
            mResourceManager->unloadLayer(mTempFieldLayers[mCurrentLayerIndex]);
            mTempFieldLayers[mCurrentLayerIndex] = nullptr;
                        
            mRenderer->setCurrentPaintTextures(layer->dataTexture(), layer->maskTexture(), layer->paletteTexture());
            //setCurrentPalette(layer->palette()->index());
            mCurrentPalette = layer->palette();
    
            auto palette = layer->palette();
            auto paletteTextureData = layer->discrete() ? palette->paletteTextureData(Palette::Type::Discrete) : palette->paletteTextureData(Palette::Type::Linear);        
            mRenderer->updateCurrentPaletteTexture(paletteTextureData, { palette->minValue(), palette->maxValue() });        
            palette->setDirty(false);
            
            mPaletteModel->setPalette(mCurrentPalette);
            //mPaletteListModel->deleteLocalPalette(tempIndex);
            mPaletteListModel->setCurrentLayerType(layer->discrete());            
    
            mDirtyFlag = true;
        }
        else if(primary > 0)
        {
            if(mTempFieldLayers[mCurrentLayerIndex] != nullptr)
            {
                mResourceManager->unloadLayer(mTempFieldLayers[mCurrentLayerIndex]);
                //auto tempIndex = mTempFieldPalettes[mCurrentLayerIndex]->index();                
                //mResourceManager->erasePalette(mTempFieldPalettes[mCurrentLayerIndex]);
                //mPaletteListModel->deleteLocalPalette(tempIndex);
                //erasePalette(mTempFieldPalettes[mCurrentLayerIndex]->index());
            }
            
            mTempFieldLayers[mCurrentLayerIndex] = mResourceManager->createLayerFromTableField(layer, layer->tableSchema()->field(primary - 1));
            
            mRenderer->setCurrentPaintTextures(mTempFieldLayers[mCurrentLayerIndex]->dataTexture(), mTempFieldLayers[mCurrentLayerIndex]->maskTexture(), mTempFieldLayers[mCurrentLayerIndex]->paletteTexture());
            mCurrentPalette = mTempFieldLayers[mCurrentLayerIndex]->palette();
            
            auto palette = mTempFieldLayers[mCurrentLayerIndex]->palette();
            auto paletteTextureData = mTempFieldLayers[mCurrentLayerIndex]->discrete() ? palette->paletteTextureData(Palette::Type::Discrete) : palette->paletteTextureData(Palette::Type::Linear);        
            mRenderer->updateCurrentPaletteTexture(paletteTextureData, { palette->minValue(), palette->maxValue() }); 
            
            mPaletteModel->setPalette(mCurrentPalette);
            //mPaletteListModel->addLocalPalette(mCurrentPalette);
            mPaletteListModel->setCurrentLayerType(mTempFieldLayers[mCurrentLayerIndex]->discrete());

            mDirtyFlag = true;
        }
    }
    
    if (primary == 0)
        layer->setSecondaryColumns(mVisibleColumnModel->secondaryColumns());
    else
        layer->clearSecondaryColumns();
}

void Chisel::setSecondaryVisbleColumn(const std::vector<int>& secondaries)
{
    
}

std::vector<std::string> Chisel::computeExpression(const std::string& expression)
{
    using namespace OpLanguage;
    using namespace antlr4;
        
    //ANTLRInputStream input(u8"Result = Otra + Adios + Adios);");
    ANTLRInputStream input(expression);
    OLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();
//     for (auto token : tokens.getTokens())
//         std::cout << token->toString() << std::endl;

    OParser parser(&tokens);
    OErrorListener errorListener;
    parser.removeErrorListeners();
    parser.addErrorListener(&errorListener);
    
    tree::ParseTree* tree = parser.program();
  
    std::unique_ptr<antlr4::tree::ParseTreeWalker> walker = std::make_unique<tree::ParseTreeWalker>();
    std::unique_ptr<Translator> trans = std::make_unique<Translator>(&tokens, this);
    walker->walk(trans.get(), tree);
    
    std::cout << tree->toStringTree(&parser) << std::endl;

    std::vector<std::string> errorStrings;
    
    if(errorListener.syntaxErrorMessages().size() || trans->semanticErrorMessages().size())
    {
        for(auto layer : trans->newLayers())
            mResourceManager->unloadLayer(layer);
        
        for(auto layer : trans->tempLayers())
            mResourceManager->unloadLayer(layer);
        
        auto errorExpression = expression;
    
        auto symbols = errorListener.offendingSymbols();
        const auto& semSymbols = trans->offendingSymbols();
        symbols.insert(begin(semSymbols), end(semSymbols));
        
        for(const auto& symbolPair : symbols)
        {
            auto token = symbolPair.second;
            errorExpression.insert(token->getStopIndex() + 1, "</span>");
            errorExpression.insert(token->getStartIndex(), "<span style=\"color: red;\">");
        }
        
        std::vector<std::string> plain = {"\n", "&", "<<", ">>"};
        std::vector<std::string> rich = {"<br>", "&amp;", "&lt;&lt;", "&gt;&gt;"};
        
        for(auto i = 0; i < plain.size(); ++i) 
            for(std::string::size_type j = 0; (j = errorExpression.find(plain[i], j)) != std::string::npos;)
            {
                errorExpression.replace(j, plain[i].length(), rich[i]);
                j += rich[i].length();
            }
        
        auto errorMessages = errorListener.syntaxErrorMessages() + trans->semanticErrorMessages();
        errorMessages.pop_back();
        
        errorStrings.push_back(errorExpression);            
        errorStrings.push_back(errorMessages);
    }
    else
    {
        for(auto layer : trans->newLayers())
        {
            mResourceManager->saveLayer(layer);
            addActiveLayer(layer);                
        }
        
        mMainWindow->selectLayer(mActiveLayerModel->index(0, 0));
        
        mDirtyFlag = true;
        
        auto i = 0;
        for(auto source: trans->sources())
        {
            std::cout << "Codigo: \n" << source << std::endl;
            auto shader = mResourceManager->createComputeShader("computeExpression" + std::to_string(i), source);
            //mRenderer->computeExpression({"computeExpression" + std::to_string(i)});
            mRenderer->computeShader(shader);
            i++;
        }

        mTempLayers = trans->tempLayers();
    }
    
    return errorStrings;
}

void Chisel::updatePaletteTexture()
{    
    updatePaletteControlPoints();

    LOG("CHISel::Updating palette texture " + mCurrentPalette->name());
    
    auto isDiscrete = mCurrentLayer->discrete();
    if(mCurrentPalette->isDirty())
    {
        LOG("CHISel::Palette texture " + mCurrentPalette->name()  + " is dirty");
        
        auto paletteType = isDiscrete ? Palette::Type::Discrete : Palette::Type::Linear;
        auto paletteTextureData = mCurrentPalette->paletteTextureData(paletteType);
        
        mRenderer->updateCurrentPaletteTexture(paletteTextureData, { mCurrentPalette->minValue(), mCurrentPalette->maxValue() });
        
        // TODO: Need to change how the renderer stores the palettes in order to make this operation more efficient
//         for(auto i = 0; i < mActiveLayers.size(); ++i)
//         {
//             if(mActiveLayers[i] != mCurrentLayer && mActiveLayers[i]->palette() == mCurrentPalette)
//             {
//                 auto data = mActiveLayers[i]->discrete() ? mCurrentPalette->paletteTextureData(Palette::Type::Discrete) : mCurrentPalette->paletteTextureData(Palette::Type::Linear);
//                 mRenderer->updatePaletteTexture(i, data, { mCurrentPalette->minValue(), mCurrentPalette->maxValue() });                    
//             }
//         }
        
//         auto search = std::find_if(begin(mActiveLayers), end(mActiveLayers), [&](const Layer* currentLayer){ return (currentLayer->palette() == mCurrentPalette && currentLayer->discrete() != isDiscrete) ? true : false;});
//         
//         if(search != end(mActiveLayers))
//         {
//             auto data = (*search)->discrete() ? mCurrentPalette->paletteTextureData(Palette::Type::Discrete) : mCurrentPalette->paletteTextureData(Palette::Type::Linear);
//             auto index = search - begin(mActiveLayers);
//             mRenderer->updatePaletteTexture(index, data, { mCurrentPalette->minValue(), mCurrentPalette->maxValue() });
//         }
                
        mCurrentPalette->setDirty(false);
    }
}

void Chisel::updatePaletteTexture(Layer* layer)
{
    if(layer->isDirty())
    {
//         auto palette = layer->palette();
//         auto paletteTextureData = layer->discrete() ? palette->paletteTextureData(Palette::Type::Discrete) : palette->paletteTextureData(Palette::Type::Linear);
//         
//         mRenderer->updateCurrentPaletteTexture(paletteTextureData, { palette->minValue(), ->maxValue() });
//         
//         mCurrentPalette->setDirty(false);
    }    
}

void Chisel::updatePaletteControlPoints()
{
    LOG("CHISel::Updating palette " + mCurrentPalette->name() + " control points");
    
    std::vector<std::tuple<std::string, int, int>> controlPointRenderingData;
    
    if(mCurrentLayer != nullptr)
    {
        auto isDiscrete = mCurrentLayer->discrete();
                
        controlPointRenderingData.push_back(std::make_tuple(toString(mCurrentPalette->maxValue(), isDiscrete), mPaletteYCoordinates.x, Alignment::Right));
        controlPointRenderingData.push_back(std::make_tuple(toString(mCurrentPalette->minValue(), isDiscrete), mPaletteYCoordinates.y, Alignment::Right | Alignment::Top));
            
        if(mCurrentPalette->controlPoints().size() > 2)
        {        
            auto controlPoints = mCurrentPalette->controlPoints();                        
            controlPoints.erase(mCurrentPalette->minValue());
            controlPoints.erase(mCurrentPalette->maxValue());
            
            auto count = 0;
            
            for(auto controlPoint : controlPoints)    
            {   
                int y;
                
                if(mCurrentPalette->isInterpolating())
                {
                    double percent;
                    
                    if(isDiscrete)
                        percent = static_cast<double>(controlPoint.first + 0.5 - mCurrentPalette->minValue()) / static_cast<double>(mCurrentPalette->maxValue() + 1 - mCurrentPalette->minValue());
                    else
                        percent = static_cast<double>(controlPoint.first - mCurrentPalette->minValue()) / static_cast<double>(mCurrentPalette->maxValue() - mCurrentPalette->minValue());
                    
                    y = static_cast<int>(ceil((1.0 - percent) * (mPaletteYCoordinates.y - mPaletteYCoordinates.x) + mPaletteYCoordinates.x));
                }
                else
                {
                    y = static_cast<int>(ceil((mPaletteYCoordinates.y - mPaletteYCoordinates.x)/(controlPoints.size() + 2.0) * (controlPoints.size() - count + 0.5) + mPaletteYCoordinates.x));
                }
                
                auto colision = false;
                for(const auto& point : controlPointRenderingData)
                {
                    auto tempY = std::get<1>(point);
                    
                    if(glm::abs(tempY - y) < 25)
                        colision = true;
                }
                
                if(!colision)
                    controlPointRenderingData.push_back(std::make_tuple(toString(controlPoint.first, isDiscrete), y, Alignment::Right | Alignment::CenteredVertical));
                
                count++;
            }
        }    
    }
    mRenderer->updateControlPointText(controlPointRenderingData);
}



void Chisel::showReadValue(float value)
{
    mGLWidget->showReadValueDialog(value, mCurrentLayer->registerType() && mTempFieldLayers[mCurrentLayerIndex] == nullptr);
}

void Chisel::hideReadValue()
{
    mGLWidget->hideReadValueDialog();
}

void Chisel::setBackgroundColor(glm::vec4 color)
{
    mRenderer->setBackgroundColor(color);
}

void Chisel::toggleVertexColorVisibility(bool checked)
{
    mRenderer->toggleVertexColorVisibility(checked);
}

void Chisel::setDefaultModelColor(glm::vec4 color)
{
    mRenderer->setDefaultModelColor(color);
}

void Chisel::setSpecularColor(glm::vec3 color)
{
    mRenderer->setSpecularColor(color);
}

void Chisel::setSpecularPower(float power)
{
    mRenderer->setSpecularPower(power);
}

void Chisel::alignMainCameraToModel()
{
    mRenderer->alignMainCameraToModel();
}


void Chisel::onMouseButtons(MouseButton button, MouseAction action, int mods, double xPos, double yPos)
{

}

void Chisel::onMousePosition(double xPos, double yPos)
{

}

void Chisel::onMouseWheel(double xOffset, double yOffset)
{

}


// *** Private Methods *** //

std::string Chisel::toString(double number, bool discrete)
{
    if(discrete)
        return std::to_string(static_cast<int>(number));
    else
        return trimZeros(std::to_string(number));
}


std::string Chisel::trimZeros(const std::string& number)
{
    auto zeroPos = number.find_last_not_of('0');
    std::string trimmed = number;
            
    if (number.at(zeroPos) == '.')
        zeroPos++;

    return trimmed.erase(zeroPos + 1, std::string::npos);    
}

