#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <memory>

class PaletteValueWidget;
class OpacityEditorWidget;
class LayerOperationWidget;
class DataBaseEditorWidget;
class LightingDialog;
class HistogramDialog;
class CellAreaDialog;
class AreaStatisticsDialog;
class NeighborStatisticsDialog;
class CostSurfaceDialog;
class OperationDialog;
class ActiveLayerModel;
class Chisel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum class State: int
    {
        Idle = 0,
        ModelLoaded,
        CreateLayer,
        EditLayer,
        LayerCreated,
        LayerLoaded,
        EditingToolsEnabled,
        EditingToolsDisabled
    };
    
    enum class Operation: unsigned int
    {
        CellArea = 0,
        AreaStats,
        NeighborStats,
        CostSurface,
        DistanceField,
        DistanceBand,
        Curvature,
        Roughness,
        Normals,
        Orientation,
        Resampling
    };
    
    MainWindow(QWidget* parent = 0);
    ~MainWindow();
    
    OpenGLWidget* visualizer() const { return mUi->Visualizer; }
    
    void settingUp();
    QSize sizeHint() const;
    
    void closeEvent(QCloseEvent* event) override;
    
    void resizeEvent(QResizeEvent* event) override;
    
    bool eventFilter(QObject* watched, QEvent* event) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override; 
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    
public slots:
    void openCreateLayerDialog();
    void fixit();
    void temp();
    
    glm::vec2 paletteYCoordinates() const;
    void openPaletteEditor(bool state);
    void updateSelectedValue(double value);
    void updateSelectedValueBasedOnPercent(double percent);
    //void movePaletteValueWidget();    
    void updatePaintingValue(double value);
    void updatePaletteTexture();
    void updatePaletteValueWidget();
    void updatePalette();
    void updatePaletteInformation();
    void updatePaletteControlPointCoordinates();
    void updatePaletteValueWidgetCoordinates();
    void updateLayerPalette();
    void selectLayer(const QModelIndex& index);
    void setCurrentLayer(const QModelIndex& current, const QModelIndex &previous);
    void changeLayerPosition(int previous, int current);
    void setCurrentPalette(const QModelIndex& current, const QModelIndex &previous);
    void toggleLayerBoxVisibility();
    void toggleLayerVisibility(const QModelIndex& index);
    void updateOpacity(unsigned int layerIndex, float opacity);

    void openOperationDialog(Operation op);
    
    void createHistogram();
    void computeCellArea(std::string name, std::pair<int, int> resolution);
    void computeAreaStatistics(unsigned int layerIndex, unsigned int fieldIndex, unsigned int baseLayerIndex, unsigned int operationIndex);
    void computeNeighborhoodStatistics(std::string name, unsigned int layerIndex, unsigned int fieldIndex, double radius, unsigned int operationIndex);
    void computeCostSurface(std::string name, unsigned int baseLayerIndex, unsigned int costLayerIndex, double radius);
    void computeDistanceField(std::string name, unsigned int baseLayerIndex, double radius);
    void computeDistanceBand(std::string name, unsigned int baseLayerIndex, double radius);
    void computeCurvature(std::string name, std::pair<int, int> resolution, double radius);
    void computeRoughness(std::string name, std::pair<int, int> resolution, double radius);
    void computeNormals(std::string name, std::pair<int, int> resolution);
    void computeOrientation(std::string name, std::pair<int, int> resolution, glm::vec3 reference);
    void computeResampling(std::string name, unsigned int layerIndex, std::pair<int, int> resolution);
    
    void updateLayerToolBoxState(int index);
    void saveNewPalette();
    void deleteNewPalette();
    void savePalette();
    void cancelEdit();
    void updateEditingActionGroupState(QAction* action);
    void changeEditingMode(QAction* action);
    
    void relocateChildrenWidget();
    void closeDataBaseEditorWidget();
    void updateVisibleColumns();
    void processSQLStatement(const std::string& sqlStatement);
    
    void setPickedValue(double value);

    void openChiselProject();
    void import3DModel();
    void exportChiselModel();
    void exportChiselProjectToUnity();
    void saveChiselProject();    
    void saveChiselProjectAs();    
    void closeChisel();
    
    int openDirtyChiselDialog();
    
    void createLayer(QString name, Layer::Type type, std::pair< int, int > resolution, int paletteIndex = -1);
    void openLayerOperationWidget();
    void duplicateLayer();
    void unloadLayer();
    void deleteLayer();
    void renameLayer(const QString& newName);
    void triggerRenameLayer();
    void computeExpression(const QString expression);
    void loadLayersSlot();
    void loadLayers(const std::vector<std::string>& layerNames);
    void exportLayerAsImage();
    void updateLayerResourceFiles();
    void saveTableSchemaToFile();
    bool enableDatabaseLayerEditing();
    
    void createPalette();
    void editPalette();
    void importPalette();
    void exportPalette();
    void duplicatePalette();
    void deletePalette();
    void addPaletteToCollection();
    void applySelectedPaletteToLayer();
    
    void editLightingParameters();
    void setSpecularColor(float value);
    void setSpecularPower(float value);
    void toggleVertexColorVisibility(bool checked);
    void selectDefaultModelColor();
    void selectBackgroundColor();
    void alignCameraToModel();

    void updateMarkToolRadius(int sliderValue);
    void updateMarkToolSlider(float radius);
    void incrementMarkToolSlider(int increment);
    
    void clearSlice();
    
protected:
    void createActions();
    void createToolBar();
    void createMenus();
    
private:
    void enableEditingTools(bool enable);
    void setState(State state);
    void setOperationUIState(bool enable);
    void setLayerUIState(bool enable);
    
    std::unique_ptr<Ui::MainWindow> mUi;
    std::unique_ptr<PaletteValueWidget> mCurrentPaletteValueWidget;
    std::unique_ptr<OpacityEditorWidget> mOpacityEditorWidget;
    std::unique_ptr<LayerOperationWidget> mLayerOperationWidget;
    std::unique_ptr<DataBaseEditorWidget> mDataBaseEditorWidget;
    LightingDialog* mLightingDialog = nullptr;
    HistogramDialog* mHistogramDialog = nullptr;
    std::unique_ptr<OperationDialog> mOperationDialog;
    QMetaObject::Connection mOperationDialogConnection;
    std::unique_ptr<QActionGroup> mEditingToolGroup;
    QAction* mLastEditingToolAction = nullptr;
    QAction* mSeparatorAction = nullptr;    
    QSlider* mMarkToolRadiusSlider = nullptr;
    QAction* mMarkToolRadiusSliderAction = nullptr;
    QPushButton* mClearSlicePlaneButton = nullptr; 
    QAction* mClearSlicePlaneButtonAction = nullptr;
    
    State mState;
    State mLastState;
    std::unique_ptr<Chisel> mChisel;
    int xSize;
};

#endif // MAINWINDOW_H
