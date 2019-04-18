#ifndef CHISEL_H
#define CHISEL_H

#include "layer.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <deque>
#include <memory>

class MainWindow;
class OpenGLWidget;
class QSqlDatabase;
class SQLiteDatabaseManager;
class DataBaseTableSchemaModel;
class DataBaseTableDataModel;
class ColumnVisibilityModel;
class VisibleColumnModel;
class DataBaseResourceModel;
class ActiveLayerModel;
class DiskLayerModel;
class PaletteListModel;
class PaletteModel;
class Palette;

class ResourceManager;
class Renderer;
class RenderTarget;
class Layer;

enum class MouseButton: unsigned int
{
    Left = 0,
    Right = 1,
    Center = 2
};

enum class MouseAction: unsigned int
{
    Press = 0,
    Move = 1, 
    Release = 2
};

struct LayerData
{
    glm::uvec2 index;
    glm::uvec2 maskIndex;
    glm::uvec2 paletteIndex;
    glm::vec2 paletteRange;
};

class Chisel
{
public:
    Chisel(MainWindow* mainWindow);
    ~Chisel();
    
    SQLiteDatabaseManager* databaseManager() const;// { return mSQLiteDatabaseManager.get(); }
    DataBaseResourceModel* dataBaseResourceModel() const { return mDataBaseResourceModel.get(); }
    DataBaseTableSchemaModel* dataBaseTableSchemaModel() const { return mDataBaseTableSchemaModel.get(); }
    DataBaseTableDataModel* dataBaseTableDataModel() const { return mDataBaseTableDataModel.get(); }
    ColumnVisibilityModel* columnVisibilityModel() const { return mColumnVisibilityModel.get(); }
    DataBaseTableDataModel* currentRowDataModel() const { return mCurrentRowDataModel.get(); }
    DataBaseTableDataModel* pickedRowDataModel() const { return mPickedRowDataModel.get(); }
    VisibleColumnModel* visibleColumnModel() const { return mVisibleColumnModel.get(); }
    ActiveLayerModel* activeLayerModel() const { return mActiveLayerModel.get(); }
    DiskLayerModel* diskLayerModel() const { return mDiskLayerModel.get(); }
    PaletteModel* paletteModel() const { return mPaletteModel.get(); }
    PaletteModel* newPaletteModel() const { return mNewPaletteModel.get(); }
    PaletteListModel* paletteListModel() const { return mPaletteListModel.get(); }
    Renderer* renderer() const { return mRenderer.get(); }
    ResourceManager* resourceManager() const { return mResourceManager.get(); }
    
    void init();
    const Layer* currentLayer() const { return mCurrentLayer; }
    const Layer* layer(unsigned int index) const { return mActiveLayers[index]; }
    const std::deque<Layer*> layers() const { return mActiveLayers; }
    const std::vector<std::string> layerNames() const;
    const std::vector<std::string> activeLayerNames(const std::set<Layer::Type>& types) const;
    const std::vector<std::pair<std::string, uint32_t>> activeLayerNameAndIndices(const std::set<Layer::Type>& types) const;
    bool hasLayers() const { return mActiveLayers.size() > 0; }
    Palette* currentPalette() const { return mCurrentPalette; }
    Palette* palette(std::string name) const;
    Palette* palette(unsigned int index) const;
    std::vector<std::string> paletteNames() const;
    //unsigned int currentPaletteIndex() const { return mCurrentPalette->index(); }
    std::string palettesPath() const;
    
    glm::vec4 backgroundColor() const;
    glm::vec4 defaultModelColor() const;
    glm::vec3 specularColor() const;
    float specularPower() const;
   
    std::string chiselName() const;
    bool isChiselPathValid() const;
    bool isSessionDirty() const { return mDirtyFlag; }
    
    void setSessionDirty(bool dirty) { mDirtyFlag = dirty; }
    void clearChiselSession();
    void updateDataBaseResourcePath(const std::string& path);
    void updateLayerPath(const std::string& path, bool clearLoadedLayers);
    void loadChiselFile(std::string name, std::string path);
    void saveChiselProject(std::string name = "", std::string path = "");
    void import3DModel(std::string name, std::string extension, std::string path);
    void exportChiselModel(std::string filePath, std::string extension, unsigned int layerIndex, bool segmentModel, bool exportCamera);
    void exportChiseProjectToUnity(std::string name = "", std::string path = "");
    
    bool setDatabase(std::string name);
   
    void createLayer(std::string name, Layer::Type type, std::pair< int, int > resolution);
    void loadLayer(std::string name);
    void duplicateLayer(unsigned int index);
    void unloadLayer(unsigned int index);
    void unloadLayer(std::string name);
    void unloadTempLayers();
    void deleteLayer(unsigned int index);
    void deleteLayer(std::string name);
    void saveLayer(unsigned int index);
    void renameLayer(unsigned int layerIndex, const std::string& newName);
    void exportLayerAsImage(unsigned int layer, const std::string& pathName);
    void insertActiveLayer(unsigned int index, Layer* layer);
    void addActiveLayer(Layer* layer);
    void propagateNewLayerData();    
    void createLayerAndUpdateInterface(std::string name, Layer::Type type, std::pair< int, int > resolution, int paletteIndex = -1);
    void setCurrentLayer(unsigned int index);
    void changeLayerPosition(int previous, int current);
    bool isLayerDirty(unsigned int index) const;
    bool isLayerAreaFieldDirty(unsigned int index) const;
    void setLayerDirty(unsigned int index);
    void setLayerAreaFieldDirty(unsigned int index);

    std::map<std::string, std::vector<uint32_t>> segmentModelWithLayer(unsigned int index);
    float computeSurfaceArea();
    float computeLayerArea(unsigned int index);
    std::vector<std::array<float, 2>> computeLayerValueArea(unsigned int index);
    void updateTableAreaFields(unsigned int index);
    
    Layer* computeCostSurfaceLayer(unsigned int seedLayerIndex, unsigned int costLayerIndex, double maxCost);
    Layer* computeDistanceFieldLayer(unsigned int index, double distance);    
    std::array<Layer*, 3> computeNormalLayer(const std::pair<int, int> layerResolution);
    Layer* computeOrientationLayer(const std::pair<int, int> layerResolution, glm::vec3 reference);
    
    void createPalette(std::string name);
    void editPalette(unsigned int index);
    void importPalette(std::string name, std::string path);
    void importPaletteToLayer(std::string name, std::string path);
    void exportPalette(unsigned int index, std::string name, std::string path); 
    void duplicatePalette(unsigned int index, bool validateName = true);
    void deletePalette(unsigned int index);
    void deleteLastPalette();
    void savePalette(unsigned int index);
    void copyPalette(unsigned int sourceIndex, unsigned int destinationIndex);
    void addPalette(Palette* palette);
    void addPaletteToCollection();
    void eraseTempPalette(Palette* palette);
    void addLocalPalette(Palette* palette);
    void addLocalPalettes(const std::vector<Palette*>& palettes);    
    void setCurrentPalette(unsigned int index);
    void setCurrentPalette(Palette* palette);
    
    void setCurrentPaintingValue(double value);
    void setPickedValue(double value);
    void setOpacity(unsigned int layer, float opacity);
    void toggleVisibility(unsigned int layer);
    void updateVisibleColumns();
    void setSecondaryVisbleColumn(const std::vector<int>& secondaries);
    std::vector<std::string> computeExpression(const std::string& expression);
   
    void updatePaletteTexture();
    void updatePaletteTexture(Layer* layer);
    void updatePaletteControlPoints();
    void showReadValue(float value);
    void hideReadValue();
    
    void setGLWidget(OpenGLWidget* glWidget) { mGLWidget = glWidget; }
    void setPaletteYCoordinates(glm::ivec2 yCoordinates) { mPaletteYCoordinates = yCoordinates; }
   
    void setBackgroundColor(glm::vec4 color);
    void toggleVertexColorVisibility(bool checked);
    void setDefaultModelColor(glm::vec4 color);
    void setSpecularColor(glm::vec3 color);
    void setSpecularPower(float power);
    void alignMainCameraToModel();
    
    void onMouseButtons(MouseButton button, MouseAction action, int mods, double xPos, double yPos);
    void onMousePosition(double xPos, double yPos);
    void onMouseWheel(double xOffset, double yOffset);
    
private:
    std::string toString(double number, bool discrete);
    std::string trimZeros(const std::string& number);    
    
    std::unique_ptr<SQLiteDatabaseManager> mSQLiteDatabaseManager;
    std::unique_ptr<DataBaseResourceModel> mDataBaseResourceModel;
    std::unique_ptr<DataBaseTableSchemaModel> mDataBaseTableSchemaModel;
    std::unique_ptr<DataBaseTableDataModel> mDataBaseTableDataModel;
    std::unique_ptr<ColumnVisibilityModel> mColumnVisibilityModel;
    std::unique_ptr<DataBaseTableDataModel> mCurrentRowDataModel;
    std::unique_ptr<DataBaseTableDataModel> mPickedRowDataModel;
    std::unique_ptr<VisibleColumnModel> mVisibleColumnModel;
    std::unique_ptr<ActiveLayerModel> mActiveLayerModel;
    std::unique_ptr<DiskLayerModel> mDiskLayerModel;
    std::unique_ptr<PaletteModel> mPaletteModel;
    std::unique_ptr<PaletteModel> mNewPaletteModel;
    std::unique_ptr<PaletteListModel> mPaletteListModel;
    std::unique_ptr<ResourceManager> mResourceManager;
    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<RenderTarget> mPaintingTarget;
    
    MainWindow* mMainWindow;
    OpenGLWidget* mGLWidget;
    glm::ivec2 mPaletteYCoordinates;
    
    bool mDirtyFlag = false;
    
    std::deque<Layer *> mActiveLayers;    
    std::vector<Layer*> mTempLayers;
    std::deque<Layer*> mTempFieldLayers;
    Layer *mCurrentLayer = nullptr;    
    unsigned int mCurrentLayerIndex = 0;
    
    Palette *mCurrentPalette = nullptr;
    unsigned int mCurrentPaletteIndex = 0;
    
    float mCurrentPaintingValue;    
    std::pair<double, glm::vec4> mCurrentPaintingData;        
    
    bool mActivePaint = false;
    bool mChangeBrushSize = false;
    bool mPaintNewColor = false;
    bool mPaintingColorNeedUpdate = false;
    
    std::unique_ptr<QSqlDatabase> mDBConnection;
};

#endif // CHISEL_H
