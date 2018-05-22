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

#include <QImage>
#include <algorithm>
#include <tuple>

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
    
    mDirtyFlag = true;
}

void Chisel::exportChiselModel(std::string filePath, std::string extension, unsigned int layerIndex, bool exportCamera)
{
    auto segmentation = segmentModelWithLayer(layerIndex);
    mResourceManager->exportSegmentedModel(filePath, extension, mRenderer->scene()->models()[0], segmentation, mRenderer->scene()->camera());
}

bool Chisel::setDatabase(std::string name)
{
    mDBConnection->setDatabaseName(QString::fromStdString(name));
    auto result = mDBConnection->open();
    return result;
}

void Chisel::createLayer(std::string name, Layer::Type type, std::pair< int, int > resolution)
{
    auto newLayer = mResourceManager->createLayer(name, type, resolution, {}, {}, mResourceManager->palettes()[0]);
    mResourceManager->saveLayer(newLayer);
    addActiveLayer(newLayer);    
    setCurrentLayer(0);
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
    insertActiveLayer(index + 1, newLayer);    
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

void Chisel::eraseLayer(unsigned int index)
{
    mActiveLayerModel->removeLayers(index, index);
    
    mResourceManager->deleteLayer(mActiveLayers[index]);
    mRenderer->eraseLayer(index);
    
    mActiveLayers.erase(begin(mActiveLayers) + index);
    
    if(!mActiveLayers.size()) 
        mCurrentLayer = nullptr;
}

void Chisel::eraseLayer(std::string name)
{
    auto search = std::find_if(begin(mActiveLayers), end(mActiveLayers), [&](const Layer* currentLayer){ return (name == currentLayer->name()) ? true : false;});

    if(search != end(mActiveLayers))
        eraseLayer(static_cast<unsigned int>(search - begin(mActiveLayers)));
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

void Chisel::insertActiveLayer(unsigned int index, Layer* layer)
{
    mDiskLayerModel->setFileState(layer->name(), true);
    
    mActiveLayers.insert(begin(mActiveLayers) + index, layer); 
    mActiveLayerModel->insertLayer(index, QString::fromStdString(layer->name()), layer->type() == Layer::Type::Register, layer->resolution(), layer->typeString());
    mTempFieldLayers.insert(begin(mTempFieldLayers) + index, nullptr);
        
    mRenderer->insertLayer(index, layer->dataTexture(), layer->maskTexture(), layer->paletteTexture());
    
    auto palette = layer->palette();
    auto paletteTextureData = layer->discrete() ? palette->paletteTextureData(Palette::Type::Discrete) : palette->paletteTextureData(Palette::Type::Linear);        
    mRenderer->updatePaletteTexture(index, paletteTextureData, { palette->minValue(), palette->maxValue() });        
    palette->setDirty(false);
    
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

float Chisel::computeSurfaceArea()
{
    auto areaTexture = mResourceManager->texture("AreaPerPixel");
    auto data = mRenderer->readFloatTexture(areaTexture);
    
    double area = 0;

    for (auto i = 0; i < data.size(); ++i)
            area += data[i];

    return static_cast<float>(area);
}

float Chisel::computeLayerArea(unsigned int index)
{
    auto areaTexture = mResourceManager->texture("AreaPerPixel");
    auto data = mRenderer->readFloatTexture(areaTexture);
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

    auto areaTexture = mResourceManager->texture("AreaPerPixel");
    auto areaData = mRenderer->readFloatTexture(areaTexture);

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
    mResourceManager->eraseLayerPalette(imported);
            
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

void Chisel::erasePalette(unsigned int index)
{
    mResourceManager->erasePalette(index);
    mPaletteListModel->delPalette(index);  
}

void Chisel::eraseLastPalette()
{
    mResourceManager->eraseLastPalette();    
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

void Chisel::exportLayerAsImage(std::string pathName, unsigned int layerIndex)
{
    auto layer = mActiveLayers[layerIndex];
    auto mask = mRenderer->readLayerMask(layerIndex);

    switch(layer->dataTexture()->type())
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        {
            exportImage<glm::byte>(pathName, layer, mRenderer->readTexture(layer->dataTexture()), mask);
            break;
        }
        case GL_SHORT:
        {
            exportImage<int16_t>(pathName, layer, mRenderer->readShortTexture(layer->dataTexture()), mask);
            break;            
        }
        case GL_UNSIGNED_SHORT:
        {
            exportImage<uint16_t>(pathName, layer, mRenderer->readUShortTexture(layer->dataTexture()), mask);
            break;            
        }
        case GL_INT:
        {
            exportImage<int32_t>(pathName, layer, mRenderer->readIntTexture(layer->dataTexture()), mask);
            break;            
        }
        case GL_UNSIGNED_INT:
        {
            exportImage<uint32_t>(pathName, layer, mRenderer->readUIntTexture(layer->dataTexture()), mask);
            break;            
        }                
        case GL_FLOAT:
        {
            exportImage<float>(pathName, layer, mRenderer->readFloatTexture(layer->dataTexture()), mask);
            break;
        }
    }    
}

template<class T>
void Chisel::exportImage(std::string pathName, Layer* layer, const std::vector<T>& data, const std::vector<glm::byte>& mask)
{
    //auto fdata = data;
    std::vector<unsigned char> pixelData(layer->width() * layer->height() * 4);
        
    for (std::size_t texel = 0; texel < mask.size(); ++texel)
    {
        if (mask[texel] > 0)
        {            
            auto color = layer->palette()->color(data[texel]) * 255;
            pixelData[4 * texel] = static_cast<unsigned char>(color.r);
            pixelData[4 * texel + 1] = static_cast<unsigned char>(color.g);
            pixelData[4 * texel + 2] = static_cast<unsigned char>(color.b);
            pixelData[4 * texel + 3] = static_cast<unsigned char>(color.a);
        }
        else
        {
            pixelData[4 * texel] = 0;
            pixelData[4 * texel + 1] = 0;
            pixelData[4 * texel + 2] = 0;
            pixelData[4 * texel + 3] = 0;
        }
    }
    
    QImage layerImage(pixelData.data(), layer->width(), layer->height(), QImage::Format_RGBA8888);
    layerImage = layerImage.mirrored(false, true);
    layerImage.save(pathName.data());    
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