#include "resourcemanager.h"
#include "mainwindow.h"

#include "sqlitedatabasemanager.h"
#include "databasetableschemamodel.h"
#include "databasetabledatamodel.h"
#include "databasetableschemaview.h"
#include "databaseresourcemodel.h"
#include "visiblecolumnmodel.h"
#include "visiblecolumnview.h"
#include "databaseeditorwidget.h"

#include "palettetableview.h"
#include "palettelistmodel.h"
#include "palettelistdelegate.h"
#include "palettemodel.h"
#include "activelayermodel.h"
#include "disklayermodel.h"
#include "chisel.h"
#include "registerlayer.h"
#include "renderer.h"

#include "logger.hpp"

#include "balloonmessagedialog.h"
#include "palettevaluewidget.h"
#include "opacityeditorwidget.h"
#include "layeroperationwidget.h"
#include "overwritedialog.h"
#include "lightingdialog.h"
#include "histogramdialog.h"
#include "fbxexporterdialog.h"

#include <chrono>

#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QMenu>
#include <QDebug>
#include <QTimer>
#include <QSignalMapper>
#include <QtWidgets/QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
:
    QMainWindow(parent)
{    
    mChisel = std::make_unique<Chisel>(this);

    mUi = std::make_unique<Ui::MainWindow>();
    mUi->setupUi(this);

    mUi->Visualizer->setFocus();
    mUi->Visualizer->setChisel(mChisel.get());
    mUi->Visualizer->setMouseTracking(false);
    mUi->Visualizer->initValueDialog(mChisel->pickedRowDataModel());
    //mUi->activeLayerTreeView->setModel(mActiveLayerModel.get());
    
    mUi->activeLayerTreeView->setModel(mChisel->activeLayerModel());
    mUi->diskLayerTreeView->setModel(mChisel->diskLayerModel());
    
    openPaletteEditor(false);    
    mUi->paletteEditor->paletteTableView()->setModel(mChisel->paletteModel());
    mUi->paletteEditor->updateButtonUsability();
    
    mUi->paletteListView->setModel(mChisel->paletteListModel());
    mUi->paletteCreator->setResourceManager(mChisel->resourceManager());
    mUi->paletteCreator->paletteTableView()->setModel(mChisel->newPaletteModel());    
    mUi->paletteCreator->hide();        
    
    //connect(mUi->paletteListView->selectionModel(), &QItemSelectionModel::selectionChanged, mUi->paletteListView, &PaletteListView::seeSelected);
    
    connect(mUi->buttonOpenPaletteEditor, &QToolButton::toggled, this, &MainWindow::openPaletteEditor);    
    
    connect(mChisel->paletteModel(), &PaletteModel::dataChanged, this, &MainWindow::updatePaletteInformation);
    connect(mChisel->paletteModel(), &PaletteModel::modelReset, this, &MainWindow::updatePaletteInformation);
    connect(mChisel->paletteModel(), &PaletteModel::interpolationChanged, this, &MainWindow::updatePaletteInformation);

    //connect(mUi->paletteListView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::setCurrentPalette);
    connect(mUi->paletteListView, &PaletteListView::doubleClicked, this, &MainWindow::applySelectedPaletteToLayer);
    connect(mUi->applyPaletteToLayerButton, &QPushButton::released, this, &MainWindow::applySelectedPaletteToLayer);
    //connect(mUi->paletteListView, &PaletteListView::newPaletteNameEdited, this, &MainWindow::saveEditedPalette);
    //connect(mUi->paletteListView, &PaletteListView::deleteNewPalette, this, &MainWindow::deleteNewPalette);
    connect(mUi->paletteCreator, &PaletteCreatorWidget::paletteCreated, this, &MainWindow::saveNewPalette);
    connect(mUi->paletteCreator, &PaletteCreatorWidget::paletteDiscarded, this, &MainWindow::deleteNewPalette);
    connect(mUi->paletteCreator, &PaletteCreatorWidget::paletteEdited, this, &MainWindow::savePalette);
    connect(mUi->paletteCreator, &PaletteCreatorWidget::editCanceled, this, &MainWindow::cancelEdit);
    
    connect(mUi->paletteEditor->paletteTableView()->selectionModel(), &QItemSelectionModel::currentChanged, mUi->paletteEditor->paletteTableView(), &PaletteTableView::changeCurrentControlPoint);
    
    connect(mUi->activeLayerTreeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::setCurrentLayer);
    connect(mChisel->activeLayerModel(), &ActiveLayerModel::layerPositionChanged, this, &MainWindow::changeLayerPosition);
    connect(mChisel->activeLayerModel(), &ActiveLayerModel::layerRenamed, this, &MainWindow::renameLayer);
    connect(mUi->activeLayerTreeView, &ActiveLayerTreeView::clicked, this, &MainWindow::toggleLayerVisibility);
    
    connect(mChisel->diskLayerModel(), &DiskLayerModel::rootPathChanged, mUi->diskLayerTreeView, &DiskLayerTreeView::updateViewWithModelData);
    connect(mUi->diskLayerTreeView, &DiskLayerTreeView::LayerSelected, this, &MainWindow::loadLayers);

    connect(mUi->layerToolBox, &QToolBox::currentChanged, this, &MainWindow::updateLayerToolBoxState);
    connect(mUi->hideLayerBoxButton, &::QToolButton::released, this, &MainWindow::toggleLayerBoxVisibility);
    
    setState(State::Idle);
}

MainWindow::~MainWindow()
{

}

// *** Public Functions *** //
void MainWindow::settingUp()
{
    mUi->Visualizer->init();
    
    mUi->graphicViewPalette->setPalette(mChisel->currentPalette());
    auto hola = mUi->graphicViewPalette->viewport()->size();
                
    mUi->Visualizer->setFocus();   
}

QSize MainWindow::sizeHint() const
{

    return QWidget::sizeHint();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    int dialogValue = QMessageBox::Discard;
    
    if(mChisel->isSessionDirty())
        dialogValue = openDirtyChiselDialog();
    
    if(dialogValue == QMessageBox::Save)
        saveChiselProject();
    
    if(dialogValue != QMessageBox::Cancel)
    {    
        log_inst.terminate_logger();
        QWidget::closeEvent(event);
    }
    else
        event->ignore();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    LOG("MainWindow::Window Resize");
    if(mChisel->currentPalette() != nullptr)
        mUi->graphicViewPalette->updatePaletteScene();
    
    auto viewport = mUi->graphicViewPalette->viewport();
    auto locPos = mUi->palettePanel->mapToParent(mUi->graphicViewPalette->pos()) + QPoint(0, viewport->pos().y());
    std::pair<int, int> range{ locPos.y(), locPos.y() + viewport->height()};

    auto coordinates = paletteYCoordinates();
    
    if(mCurrentPaletteValueWidget != nullptr)
        mCurrentPaletteValueWidget->setCoordinates(locPos.x(), range);

    if(mDataBaseEditorWidget != nullptr && mDataBaseEditorWidget->isVisible())
        mDataBaseEditorWidget->move(QPoint(mUi->Visualizer->pos().x(), mDataBaseEditorWidget->pos().y()));

    if(mOpacityEditorWidget != nullptr && mOpacityEditorWidget->isVisible())
        mOpacityEditorWidget->move(QPoint(mUi->Visualizer->pos().x(), mOpacityEditorWidget->pos().y()));

    if(mLayerOperationWidget != nullptr && mLayerOperationWidget->isVisible())
        mLayerOperationWidget->move(QPoint(mUi->Visualizer->pos().x(), mLayerOperationWidget->pos().y()));
            
    mChisel->setPaletteYCoordinates(coordinates);
    
    if(mChisel->currentLayer() != nullptr)
    {
        mUi->Visualizer->makeCurrent();
        mChisel->updatePaletteControlPoints();
    }
    
    if(mMarkToolRadiusSlider != nullptr)
    {
        auto max = std::min(mUi->Visualizer->width(), mUi->Visualizer->height()) / 2;
        auto radius = mChisel->renderer()->markToolRadius();
        
        if(radius > max)
            updateMarkToolRadius(mMarkToolRadiusSlider->maximum());
        else
            updateMarkToolSlider(radius);        
    }
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{        
    if(watched == mUi->Visualizer)
    {
        if(event->type() == QEvent::Resize)
        {
            LOG("MainWindow::Event Filter::Visualizer resize");
            auto viewport = mUi->graphicViewPalette->viewport();
            auto locPos = mUi->palettePanel->mapToParent(mUi->graphicViewPalette->pos()) + QPoint(0, viewport->pos().y());
            std::pair<int, int> range{ locPos.y(), locPos.y() + viewport->height()};

            auto coordinates = paletteYCoordinates();
            
            if(mCurrentPaletteValueWidget != nullptr)
                mCurrentPaletteValueWidget->setCoordinates(locPos.x(), range);

            if(mDataBaseEditorWidget != nullptr && mDataBaseEditorWidget->isVisible())
                mDataBaseEditorWidget->move(QPoint(mUi->Visualizer->pos().x(), mDataBaseEditorWidget->pos().y()));
            
            if(mOpacityEditorWidget != nullptr && mOpacityEditorWidget->isVisible())
                mOpacityEditorWidget->move(QPoint(mUi->Visualizer->pos().x(), mOpacityEditorWidget->pos().y()));
            
            if(mLayerOperationWidget != nullptr && mLayerOperationWidget->isVisible())
                mLayerOperationWidget->move(QPoint(mUi->Visualizer->pos().x(), mLayerOperationWidget->pos().y()));
            
            mChisel->setPaletteYCoordinates(coordinates);
            
            if(mChisel->currentLayer() != nullptr)
            {
                mUi->Visualizer->makeCurrent();
                mChisel->updatePaletteControlPoints();
            }
            
            if(mMarkToolRadiusSlider != nullptr)
            {
                auto max = std::min(mUi->Visualizer->width(), mUi->Visualizer->height()) / 2;
                auto radius = mChisel->renderer()->markToolRadius();
                
                if(radius > max)
                    updateMarkToolRadius(mMarkToolRadiusSlider->maximum());
                else
                    updateMarkToolSlider(radius);        
            }
            
            return false;
        }
    }
        
    return QMainWindow::eventFilter(watched, event);        
}


void MainWindow::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
//     if(mUpdatingMarkToolRadius)        
//     {
//         auto deltaY = event->angleDelta().ry() / 3;
//         mMarkToolRadiusSlider->setValue(mMarkToolRadiusSlider->value() + deltaY);
//     }
//     else
        QWidget::wheelEvent(event);
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{
//     if(mChisel->renderer()->isMarkModeActive() || mChisel->renderer()->isEraseModeActive())
//     {
//         if(event->key() == Qt::Key_S)
//             mUpdatingMarkToolRadius = true;
//     }
//     else
        QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
//     if(mUpdatingMarkToolRadius && event->key() == Qt::Key_S)
//         mUpdatingMarkToolRadius = false;
//     
    QWidget::keyReleaseEvent(event);
}

// *** Public Slots *** //
void MainWindow::openCreateLayerDialog()
{
    setState(State::CreateLayer);
    
    CreateLayerDialog createLayer(mChisel->diskLayerModel(), this);
    
    connect(&createLayer, &CreateLayerDialog::createNewLayerSignal, this, &MainWindow::createLayer);
    
    createLayer.exec();
}

void MainWindow::fixit()
{    
    mLayerOperationWidget = std::make_unique<LayerOperationWidget>(mUi->centralwidget);
    auto index = mUi->activeLayerTreeView->model()->index(0, 0);
    auto posY = mUi->activeLayerTreeView->mapTo(mUi->centralwidget, mUi->activeLayerTreeView->visualRect(index).center()).y() + 2;
    auto posX = mUi->Visualizer->pos().x();
    mLayerOperationWidget->move({posX, posY - mLayerOperationWidget->height()/2});    
    mLayerOperationWidget->setMaxAndMinSizes({500, 40}, {1280, 720});
    
    connect(mLayerOperationWidget.get(), &LayerOperationWidget::statementFinished, this, &MainWindow::computeExpression);

    createActions();
    createToolBar();
    createMenus();

    mUi->activeLayerTreeView->setupSectionSize();
    
    mUi->graphicViewPalette->updatePaletteScene();
    mUi->graphicViewPalette->setSceneRect(mUi->graphicViewPalette->scene()->itemsBoundingRect());

    auto viewport = mUi->graphicViewPalette->viewport();
    auto viewSize = viewport->size();
    auto locPos = mUi->palettePanel->mapToParent(mUi->graphicViewPalette->pos()) + QPoint(0, viewport->pos().y());
    std::pair<int, int> range{ locPos.y(), locPos.y() + viewport->height()};
    mChisel->setPaletteYCoordinates({ mUi->Visualizer->height() - (locPos.y() + 1 - mUi->Visualizer->pos().y() + viewport->height()), mUi->Visualizer->height() - (locPos.y() + 1 - mUi->Visualizer->pos().y())});

    mCurrentPaletteValueWidget = std::make_unique<PaletteValueWidget>(mChisel->currentPalette(), mChisel->currentRowDataModel(), locPos.x(), range, mUi->centralwidget);    
    mCurrentPaletteValueWidget->resize(QSize(100, 42));
    mCurrentPaletteValueWidget->show();
    mCurrentPaletteValueWidget->move(locPos - QPoint(mCurrentPaletteValueWidget->width(), mCurrentPaletteValueWidget->height() / 2.0));
    updatePaletteInformation();
    mCurrentPaletteValueWidget->hide();
    
//     connect(mUi->paletteEditor->paletteTableView(), &PaletteTableView::newCurrentControlPoint, this, &MainWindow::updateSelectedValue);
    connect(mUi->graphicViewPalette, &PaletteGraphicsView::clickedValue, this, &MainWindow::updateSelectedValueBasedOnPercent);

    connect(mCurrentPaletteValueWidget.get(), &PaletteValueWidget::paintingValueChanged, this, &MainWindow::updatePaintingValue);
    connect(mCurrentPaletteValueWidget.get(), &PaletteValueWidget::paletteChanged, this, &MainWindow::updatePalette);
    
    mCurrentPaletteValueWidget->updatePaintingData();

    mDataBaseEditorWidget = std::make_unique<DataBaseEditorWidget>(mChisel->dataBaseTableSchemaModel(), mChisel->dataBaseTableDataModel(), mChisel->columnVisibilityModel(), mChisel->visibleColumnModel(), mChisel->dataBaseResourceModel(), mUi->centralwidget);
    mDataBaseEditorWidget->setFixedTipPosition(true); 
    connect(mDataBaseEditorWidget.get(), &DataBaseEditorWidget::editingFinished, this, &MainWindow::closeDataBaseEditorWidget);
    connect(mDataBaseEditorWidget.get(), &DataBaseEditorWidget::schemaEditingFinished, this, &MainWindow::relocateChildrenWidget);
    connect(mDataBaseEditorWidget.get(), &DataBaseEditorWidget::resized, this, &MainWindow::relocateChildrenWidget);
    connect(mDataBaseEditorWidget.get(), &DataBaseEditorWidget::moved, this, &MainWindow::relocateChildrenWidget);
    connect(mDataBaseEditorWidget.get(), &DataBaseEditorWidget::resourceImported, this, &MainWindow::updateLayerResourceFiles);
    connect(mDataBaseEditorWidget->dataBaseTableView(), &DataBaseTableDataView::newIdSelected, this, &MainWindow::updateSelectedValue);

    connect(mChisel->visibleColumnModel(), &VisibleColumnModel::dataChanged, this, &MainWindow::updateVisibleColumns);    

//     connect(mChisel->currentRowDataModel(), &DataBaseTableDataModel::dataCommitted, mChisel->dataBaseTableDataModel(), &DataBaseTableDataModel::select);
    connect(mChisel->currentRowDataModel(), &DataBaseTableDataModel::dataCommitted, mChisel->pickedRowDataModel(), &DataBaseTableDataModel::select);
    connect(mChisel->dataBaseTableDataModel(), &DataBaseTableDataModel::tableSchemaModified, this, &MainWindow::saveTableSchemaToFile);
    connect(mChisel->dataBaseTableDataModel(), &DataBaseTableDataModel::dataCommitted, this, &MainWindow::enableDatabaseLayerEditing);    
    connect(mChisel->dataBaseTableDataModel(), &DataBaseTableDataModel::tableSchemaModified, mChisel->pickedRowDataModel(), &DataBaseTableDataModel::loadTable);
    connect(mChisel->dataBaseTableDataModel(), &DataBaseTableDataModel::dataCommitted, mCurrentPaletteValueWidget.get(), &PaletteValueWidget::updateTableRow);
    connect(mChisel->dataBaseTableDataModel(), &DataBaseTableDataModel::dataCommitted, mChisel->pickedRowDataModel(), &DataBaseTableDataModel::select);


    mOpacityEditorWidget = std::make_unique<OpacityEditorWidget>(0, 1.0, mUi->centralwidget);
    mOpacityEditorWidget->resize(QSize(200, 40));
    connect(mOpacityEditorWidget.get(), &OpacityEditorWidget::opacityUpdated, this, &MainWindow::updateOpacity);

    mUi->Visualizer->installEventFilter(this);    
}

glm::vec2 MainWindow::paletteYCoordinates() const
{
    auto viewport = mUi->graphicViewPalette->viewport();    
    auto locPos = mUi->palettePanel->mapToParent(mUi->graphicViewPalette->pos()) + QPoint(0, viewport->pos().y());
    
    return { mUi->Visualizer->height() - (locPos.y() + 1 - mUi->Visualizer->pos().y() + viewport->height()), mUi->Visualizer->height() - (locPos.y() + 1 - mUi->Visualizer->pos().y())};
}


void MainWindow::openPaletteEditor(bool state)
{
    LOG("toolBoxLayer size: ", mUi->layerToolBox->size().width(), ", ", mUi->layerToolBox->size().height());
    
    if(state)
    {
        //mUi->paletteEditor->setVisible(true);        
        mUi->toolBoxPalette->setVisible(true);
        mUi->toolBoxPalette->setItemText(1, QString::fromStdString(mChisel->currentPalette()->name()));
        mUi->toolBoxPalette->setCurrentIndex(1);
        mUi->paletteEditor->setFocus();
    }
    else
    {
        mUi->Visualizer->setFocus();
        mUi->toolBoxPalette->hide();
        
        mUi->palettePanel->adjustSize();
        
        if(!isMaximized())
        {
            centralWidget()->adjustSize();        
            QTimer::singleShot(0, this, &MainWindow::temp);
        }
        //mUi->paletteEditor->hide();
    }
}

void MainWindow::updateSelectedValue(double value)
{
    mCurrentPaletteValueWidget->setNewValue(value);
    mChisel->setCurrentPaintingValue(value);
}

void MainWindow::updateSelectedValueBasedOnPercent(double percent)
{
    if(mChisel->currentLayer() != nullptr)
        if(!mChisel->currentLayer()->registerType())
            mCurrentPaletteValueWidget->updateBasedOnPercent(percent);
}

void MainWindow::temp()
{
    this->resize(minimumSizeHint());
}


// void MainWindow::movePaletteValueWidget()
// {
//     mCurrentPaletteValueWidget->move({mUi->graphicViewPalette->pos().x() - mCurrentPaletteValueWidget->width(), mCurrentPaletteValueWidget->pos().y()});
// }

void MainWindow::updatePaintingValue(double value)
{
    mChisel->setCurrentPaintingValue(value);
    mUi->Visualizer->setFocus();
}

void MainWindow::updatePaletteTexture()
{
    mUi->Visualizer->makeCurrent();
    mChisel->updatePaletteTexture();
}

void MainWindow::updatePaletteValueWidget()
{
    if(mChisel->currentLayer()->discrete())
        mCurrentPaletteValueWidget->setIntValidator(static_cast<int>(mChisel->currentPalette()->minValue()), static_cast<int>(mChisel->currentPalette()->maxValue()));
    else
        mCurrentPaletteValueWidget->setDoubleValidator(mChisel->currentPalette()->minValue(), mChisel->currentPalette()->maxValue());
    
    if(mCurrentPaletteValueWidget->palette() != mChisel->currentPalette())
        mCurrentPaletteValueWidget->setPalette(mChisel->currentPalette());       
    else
        mCurrentPaletteValueWidget->updatePaintingData();
}

void MainWindow::updatePalette()
{
    mChisel->paletteModel()->setPalette(mChisel->currentPalette());    
}

void MainWindow::updatePaletteInformation()
{
    LOG("MainWindow::Update Palette Information: ", mChisel->currentPalette()->name());
    
    
//     QSignalBlocker blocker(mUi->paletteListView->selectionModel());
// // /*    if(mChisel->currentPalette()->isDirty())
// //         mUi->paletteListView->setCurrentIndex(QModelIndex());
// //     else */
// //         mUi->paletteListView->selectionModel()->select(mChisel->paletteListModel()->index(static_cast<int>(mChisel->currentPaletteIndex()), 0), QItemSelectionModel::Select);
//         mUi->paletteListView->setCurrentIndex(mChisel->paletteListModel()->index(static_cast<int>(mChisel->currentPaletteIndex()), 0));
//     //mUi->paletteListView->setCurrentIndex(QModelIndex());
//     blocker.unblock();
    
    mUi->paletteEditor->updateButtonUsability();
    mUi->paletteEditor->setInterpolationButtonChecked(mChisel->currentPalette()->isInterpolating());
    
    if(mUi->graphicViewPalette->palette() != mChisel->currentPalette())
        mUi->graphicViewPalette->setPalette(mChisel->currentPalette());
    else
        mUi->graphicViewPalette->updatePaletteScene();
        
    if (mChisel->currentLayer() != nullptr)
    {
        if (mChisel->currentLayer()->discrete())
            mCurrentPaletteValueWidget->setIntValidator(static_cast<int>(mChisel->currentPalette()->minValue()), static_cast<int>(mChisel->currentPalette()->maxValue()));
        else
            mCurrentPaletteValueWidget->setDoubleValidator(mChisel->currentPalette()->minValue(), mChisel->currentPalette()->maxValue());
    }
    else
        mCurrentPaletteValueWidget->setIntValidator(static_cast<int>(mChisel->currentPalette()->minValue()), static_cast<int>(mChisel->currentPalette()->maxValue()));


    if(mCurrentPaletteValueWidget->palette() != mChisel->currentPalette())
        mCurrentPaletteValueWidget->setPalette(mChisel->currentPalette());       
    else
        mCurrentPaletteValueWidget->updatePaintingData();
    
    if(!mChisel->currentPalette()->isInterpolating())
        mCurrentPaletteValueWidget->setControlPointValidator();

    mUi->Visualizer->makeCurrent();
    if(mChisel->currentLayer() != nullptr)
        mChisel->updatePaletteTexture();    
}

void MainWindow::updatePaletteControlPointCoordinates()
{
    auto coordinates = paletteYCoordinates();
    mChisel->setPaletteYCoordinates(coordinates);

    mUi->Visualizer->makeCurrent();
    mChisel->updatePaletteControlPoints(); 
}

void MainWindow::updatePaletteValueWidgetCoordinates()
{
    auto viewport = mUi->graphicViewPalette->viewport();
    auto locPos = mUi->palettePanel->mapToParent(mUi->graphicViewPalette->pos()) + QPoint(0, viewport->pos().y());
    std::pair<int, int> range { locPos.y(), locPos.y() + viewport->height()};

    mCurrentPaletteValueWidget->setCoordinates(locPos.x(), range);
}



void MainWindow::updateLayerPalette()
{
    openPaletteEditor(true);
}

void MainWindow::selectLayer(const QModelIndex& index)
{
    LOG("MainWindow::SelectLayer index: ", index.row());
    mUi->activeLayerTreeView->selectionModel()->clearSelection();
    mUi->activeLayerTreeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
}

void MainWindow::setCurrentLayer(const QModelIndex& current, const QModelIndex& previous)
{    
    if(current.isValid())
    {
        auto row = !current.parent().isValid() ? current.row() : current.parent().row();
        LOG("MainWindow::Set Current Layer index: ", row);
        mUi->Visualizer->makeCurrent();
        mChisel->setCurrentLayer(row);

        if(previous.isValid() && current != previous)
        {
            auto previousRow = !previous.parent().isValid() ? previous.row() : previous.parent().row();
            
            if(mChisel->layer(previousRow)->registerType() && (mChisel->renderer()->isMarkModeActive() || mChisel->renderer()->isEraseModeActive()))
                mChisel->updateTableAreaFields(previousRow);
        }
        
        mUi->Visualizer->hideReadValueDialog();
        
        mUi->graphicViewPalette->updatePaletteScene();
        
        if(mChisel->currentLayer()->discrete())
        {
            mCurrentPaletteValueWidget->setIntValidator(static_cast<int>(mChisel->currentPalette()->minValue()), static_cast<int>(mChisel->currentPalette()->maxValue()));
            
            if (mChisel->currentLayer()->registerType())
            {                                
                auto columnVisibility = dynamic_cast<const RegisterLayer *>(mChisel->currentLayer())->secondaryColumns();
                
                auto regLayer = dynamic_cast<const RegisterLayer*>(mChisel->currentLayer());
                if(regLayer->hasResources())
                    mDataBaseEditorWidget->setResourceDirectory(regLayer->name());
                else
                    mDataBaseEditorWidget->clearResourceDirectorySelection();
             
                if(!mChisel->currentLayer()->isVisible())
                {
                    enableEditingTools(false);
                    mUi->actionValuePickerTool->setDisabled(true);
                }
                else
                    mUi->actionValuePickerTool->setEnabled(true);
                
                if(enableDatabaseLayerEditing())
                {
                    auto valid = mDataBaseEditorWidget->selectId(static_cast<int>(mCurrentPaletteValueWidget->value()));
                    mDataBaseEditorWidget->updateColumnVisibility();
                    
                    if(!valid)
                        mDataBaseEditorWidget->selectId(0);
                    
                    if(mUi->actionMarkTool->isChecked())
                        mDataBaseEditorWidget->show();
                    
                    mCurrentPaletteValueWidget->setRowViewVisible(true);               
                    mCurrentPaletteValueWidget->setRowViewColumnVisibility(columnVisibility);
                    mCurrentPaletteValueWidget->updatePaintingData();
                }
            }
            else
            {   
                if(mChisel->currentLayer()->isVisible())
                {
                    enableEditingTools(true);
                    mUi->actionValuePickerTool->setEnabled(true);
                }
                else
                {
                    enableEditingTools(false);
                    mUi->actionValuePickerTool->setDisabled(true);
                }
                
                mCurrentPaletteValueWidget->setRowViewVisible(false);
                mDataBaseEditorWidget->hide();
            }

            mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Int);
        }
        else
        {
            if(mChisel->currentLayer()->isVisible())
            {
                enableEditingTools(true);
                mUi->actionValuePickerTool->setEnabled(true);                
            }
            else
            {
                enableEditingTools(false);
                mUi->actionValuePickerTool->setDisabled(true);
            }
            
            mCurrentPaletteValueWidget->setDoubleValidator(mChisel->currentPalette()->minValue(), mChisel->currentPalette()->maxValue());
            mCurrentPaletteValueWidget->setRowViewVisible(false);          
            
            mDataBaseEditorWidget->hide();
                
            mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Double);
        }
        
        mCurrentPaletteValueWidget->updatePaintingData();
    }
}

void MainWindow::changeLayerPosition(int previous, int current)
{
    mUi->Visualizer->makeCurrent();
    mChisel->changeLayerPosition(previous, current);
    mUi->Visualizer->update();
}


void MainWindow::setCurrentPalette(const QModelIndex& current, const QModelIndex& previous)
{    
    LOG("MainWindow::Set current Palette index: ", current.row());        
    mUi->Visualizer->makeCurrent();

//     if(!mChisel->currentPalette()->isDirty())
//         mChisel->eraseTempPalette(mChisel->currentPalette());
    
    mChisel->setCurrentPalette(current.row());
    
    mUi->toolBoxPalette->setItemText(1, QString::fromStdString(mChisel->currentPalette()->name()));
    
    //mUi->Visualizer->update();
    
//     mUi->graphicViewPalette->setPalette(mChisel->currentPalette());
//     mCurrentPaletteValueWidget->setPalette(mChisel->currentPalette());
}

void MainWindow::toggleLayerBoxVisibility()
{
    if(mUi->layerToolBox->isVisible())
    {
        mUi->layerToolBox->hide();
        mUi->hideLayerBoxButton->setArrowType(Qt::ArrowType::RightArrow);
    }
    else
    {
        mUi->layerToolBox->show();
        mUi->hideLayerBoxButton->setArrowType(Qt::ArrowType::LeftArrow);
    }
}


void MainWindow::toggleLayerVisibility(const QModelIndex& index)
{
    if(index.column() == 1 && mChisel->currentLayer()->registerType())
    {
        if(mOpacityEditorWidget->isVisible()) mOpacityEditorWidget->hide();
        if(mLayerOperationWidget->isVisible()) mLayerOperationWidget->hideWidget();
        
        if(mDataBaseEditorWidget->isHidden())
            mDataBaseEditorWidget->show();
        else if(mDataBaseEditorWidget->layerIndex() == index.row())
            mDataBaseEditorWidget->hide();        
    }    
    else if(index.column() == 2)
    {    
        mUi->Visualizer->makeCurrent();
        mChisel->toggleVisibility(index.row());
        if(mChisel->layer(index.row())->isVisible())
        {
            enableEditingTools(true);
            mUi->actionValuePickerTool->setEnabled(true);
            if(mChisel->layer(index.row())->registerType()) enableDatabaseLayerEditing();
        }
        else
        {
            enableEditingTools(false);
            mUi->actionValuePickerTool->setDisabled(true);
        }
        
        //updateVisibleColumns();
        
        mUi->Visualizer->update();        
    }
    else if(index.column() == 3)
    {
        if(mDataBaseEditorWidget->isVisible()) mDataBaseEditorWidget->hide();
        if(mLayerOperationWidget->isVisible()) mLayerOperationWidget->hideWidget();
        
        if(mOpacityEditorWidget->isHidden())
            mOpacityEditorWidget->show();
        else if(mOpacityEditorWidget->layerIndex() == index.row())
            mOpacityEditorWidget->hide();
    }
    
    if(mChisel->currentLayer()->registerType() && mDataBaseEditorWidget->isVisible())
    {
        mDataBaseEditorWidget->setLayer(index.row());
        
        auto posY = mUi->activeLayerTreeView->mapTo(mUi->centralwidget, mUi->activeLayerTreeView->visualRect(index).center()).y() + 2;
        auto posX = mUi->Visualizer->pos().x();
        mDataBaseEditorWidget->resetTipPosition();
        mDataBaseEditorWidget->move(QPoint(posX, posY) - QPoint(0, mDataBaseEditorWidget->tipPosition().y()));       
    }
    else if(mOpacityEditorWidget->isVisible())
    {
        mOpacityEditorWidget->setLayer(index.row(), mChisel->layer(index.row())->opacity());
            
        auto posY = mUi->activeLayerTreeView->mapTo(mUi->centralwidget, mUi->activeLayerTreeView->visualRect(index).center()).y() + 2;
        auto posX = mUi->Visualizer->pos().x();
        mOpacityEditorWidget->move(QPoint(posX, posY) - QPoint(0, mOpacityEditorWidget->height() / 2.0));        
    }
}

void MainWindow::updateOpacity(unsigned int layerIndex, float opacity)
{
    mUi->Visualizer->makeCurrent();
    mChisel->setOpacity(layerIndex, opacity);
    mUi->Visualizer->update();     
}

void MainWindow::createHistogram()
{
    auto index = mUi->activeLayerTreeView->currentIndex();
    mUi->Visualizer->makeCurrent();

    mHistogramDialog = new HistogramDialog(mChisel.get(), index.row(), this);
    mHistogramDialog->setAttribute(Qt::WA_DeleteOnClose);

    mHistogramDialog->resize(1600, 900);
    mHistogramDialog->show();
}

void MainWindow::updateLayerToolBoxState(int index)
{
    if(index == 0)
    {
        mDataBaseEditorWidget->hide();
        mOpacityEditorWidget->hide();        
    }
}

void MainWindow::saveNewPalette()
{
    mUi->paletteListView->update();
    auto paletteCount = mUi->paletteListView->model()->rowCount();    
    auto index = mUi->paletteListView->model()->index(paletteCount - 1, 0);
    mUi->paletteListView->setCurrentIndex(index);
    
    mChisel->savePalette(paletteCount - 1);
    
    mUi->paletteButtonWidget->show();
    mUi->paletteListView->show();
    mUi->applyPaletteToLayerButton->show();
    mUi->paletteCreator->hide();
}

void MainWindow::deleteNewPalette()
{
    mChisel->eraseLastPalette();
    
    mUi->paletteButtonWidget->show();
    mUi->paletteListView->show();
    mUi->applyPaletteToLayerButton->show();
    mUi->paletteCreator->hide();     
}

void MainWindow::savePalette()
{
    auto paletteCount = mUi->paletteListView->model()->rowCount();    
    auto lastIndex = mUi->paletteListView->model()->index(paletteCount - 1, 0).row();
    auto currentIndex = mUi->paletteListView->currentIndex().row();
    
    mChisel->copyPalette(lastIndex, currentIndex);    
    mChisel->savePalette(currentIndex);
    mChisel->eraseLastPalette();

    mUi->paletteListView->update();

    mUi->paletteButtonWidget->show();
    mUi->paletteListView->show();
    mUi->applyPaletteToLayerButton->show();
    mUi->paletteCreator->hide();
}

void MainWindow::cancelEdit()
{
    mChisel->eraseLastPalette();
    
    mUi->paletteButtonWidget->show();
    mUi->paletteListView->show();
    mUi->applyPaletteToLayerButton->show();    
    mUi->paletteCreator->hide();
}

void MainWindow::updateEditingActionGroupState(QAction* action)
{
    if (action == mLastEditingToolAction)
    {
        action->setChecked(false);
        mLastEditingToolAction = nullptr;
    }
    else
        mLastEditingToolAction = action;  
}

void MainWindow::changeEditingMode(QAction* action)
{
    mUi->Visualizer->makeCurrent();
    
    if((mChisel->renderer()->isMarkModeActive() || mChisel->renderer()->isEraseModeActive()) 
        && (action != mUi->actionMarkTool && action != mUi->actionEraserTool) && mChisel->currentLayer()->registerType())
    {
        auto index = mUi->activeLayerTreeView->currentIndex();
        mChisel->updateTableAreaFields(index.row());
    }
    
    if(mChisel->renderer()->isSliceModeActive())
        mChisel->renderer()->sliceModelWithPlane();
    
    if(action == mUi->actionMarkTool)
    {
        mUi->Visualizer->setCursor(QCursor(Qt::ArrowCursor));
        mUi->Visualizer->hideReadValueDialog();
        
        mClearSlicePlaneButtonAction->setVisible(false);
        
        auto active = mChisel->renderer()->isMarkModeActive();

        if(active)
        {
            mCurrentPaletteValueWidget->hide();
            mSeparatorAction->setVisible(false);
            mMarkToolRadiusSliderAction->setVisible(false);
        }
        else
        {
            mCurrentPaletteValueWidget->show();
            mSeparatorAction->setVisible(true);
            mMarkToolRadiusSliderAction->setVisible(true);
        }
        
        if(mChisel->currentLayer()->registerType())
        {            
            if(!active)
            {
                mDataBaseEditorWidget->selectEditingMode();
                mDataBaseEditorWidget->show();
                if(mDataBaseEditorWidget->dataBaseTableView()->selectionModel()->selectedIndexes().size() == 0)
                {
                    auto valid = mDataBaseEditorWidget->selectId(static_cast<int>(mCurrentPaletteValueWidget->value()));                
                    if(!valid)
                        mDataBaseEditorWidget->selectId(0);
                }
            }
        }

        mChisel->renderer()->setMarkMode(!active);
        if(!active && mUi->Visualizer->underMouse())
            mChisel->renderer()->insertMarkTool();        
    }
    else if (action == mUi->actionEraserTool)
    {
        mCurrentPaletteValueWidget->hide();
        
        auto active = mChisel->renderer()->isEraseModeActive();
        mSeparatorAction->setVisible(!active);
        mMarkToolRadiusSliderAction->setVisible(!active);
        mClearSlicePlaneButtonAction->setVisible(false);
            
        mUi->Visualizer->setCursor(QCursor(Qt::ArrowCursor));
        mUi->Visualizer->hideReadValueDialog();
                
        mChisel->renderer()->setEraseMode(!active);
        if(!active && mUi->Visualizer->underMouse())
            mChisel->renderer()->insertMarkTool();
    }
    else if (action == mUi->actionPickerTool)
    {
        auto active = mChisel->renderer()->isPickModeActive();
                
        (!active) ? mUi->Visualizer->setCursor(QCursor(Qt::CrossCursor)) : mUi->Visualizer->setCursor(QCursor(Qt::ArrowCursor));
        mCurrentPaletteValueWidget->hide();
        mSeparatorAction->setVisible(false);
        mMarkToolRadiusSliderAction->setVisible(false);
        mClearSlicePlaneButtonAction->setVisible(false);
        mUi->Visualizer->hideReadValueDialog();
        
        mChisel->renderer()->setPickMode(!active);
    }
    else if (action == mUi->actionValuePickerTool)
    {        
        auto active = mChisel->renderer()->isReadModeActive();
        
        if(!active)
            mUi->Visualizer->setCursor(QCursor(Qt::CrossCursor));
        else
        {            
            mUi->Visualizer->setCursor(QCursor(Qt::ArrowCursor));
            mUi->Visualizer->hideReadValueDialog();   
        }
        
        mCurrentPaletteValueWidget->hide();
        mSeparatorAction->setVisible(false);        
        mMarkToolRadiusSliderAction->setVisible(false);
        mClearSlicePlaneButtonAction->setVisible(false);
        
        mChisel->renderer()->setReadMode(!active);
    }
    else if (action == mUi->actionSliceModelTool)
    {
        mCurrentPaletteValueWidget->hide();        
        mMarkToolRadiusSliderAction->setVisible(false);
                
        mUi->Visualizer->setCursor(QCursor(Qt::ArrowCursor));
        mUi->Visualizer->hideReadValueDialog();
        
        auto active = mChisel->renderer()->isSliceModeActive();
        
        if(!active)
        {
            mSeparatorAction->setVisible(true);
            mClearSlicePlaneButtonAction->setVisible(true);            
        }
        else
        {
            mSeparatorAction->setVisible(false);
            mClearSlicePlaneButtonAction->setVisible(false);
        }
        
        mChisel->renderer()->setSliceMode(!active);
    }
    
    mUi->Visualizer->update();
}

void MainWindow::relocateChildrenWidget()
{
    auto sourcePos = mDataBaseEditorWidget->pos();
    auto sourceSize = mDataBaseEditorWidget->size();
    auto targetPos = mUi->Visualizer->pos();
    auto targetSize = mUi->Visualizer->size();

    if(sourcePos.y() < (targetPos.y() + 1) || sourcePos.x() < targetPos.x())
    {
        auto posY = mUi->Visualizer->geometry().top() + 1 - mDataBaseEditorWidget->geometry().top();
        auto posX = mUi->Visualizer->pos().x();
//         mDataBaseEditorWidget->setTipPosition(mDataBaseEditorWidget->tipPosition() - QPoint(0, posY));
//         mDataBaseEditorWidget->updateShapes();
        mDataBaseEditorWidget->move(QPoint(posX, mDataBaseEditorWidget->pos().y() + posY));         
    }
    else
    if ((sourcePos.y() + sourceSize.height()) > (targetPos.y() + targetSize.height()) ||
        (sourcePos.x() + sourceSize.width()) > (targetPos.x() + targetSize.width()))
    {
        auto posY = mUi->Visualizer->geometry().bottom() - (mDataBaseEditorWidget->geometry().bottom() + 1);
        auto posX = mUi->Visualizer->pos().x();
        //mDataBaseEditorWidget->setTipPosition(mDataBaseEditorWidget->tipPosition() - QPoint(0, posY));

        mDataBaseEditorWidget->move(QPoint(posX, mDataBaseEditorWidget->pos().y() + posY));    
    }
    
    if (mDataBaseEditorWidget->tipPosition().y() > mDataBaseEditorWidget->height() / 2)
    {
        auto posY = mDataBaseEditorWidget->tipPosition().y() - mDataBaseEditorWidget->height() / 2;
        auto posX = mUi->Visualizer->pos().x();
        mDataBaseEditorWidget->setTipPosition(mDataBaseEditorWidget->tipPosition() - QPoint(0, posY));
        mDataBaseEditorWidget->move(QPoint(posX, mDataBaseEditorWidget->pos().y() + posY));
    }

    auto index = mUi->activeLayerTreeView->currentIndex();
    auto layerY = mUi->activeLayerTreeView->mapTo(mUi->centralwidget, mUi->activeLayerTreeView->visualRect(index).center()).y() + 2;
    auto tipY = mDataBaseEditorWidget->mapTo(mUi->centralwidget, mDataBaseEditorWidget->tipPosition()).y();
    
    if(layerY != tipY)
        mDataBaseEditorWidget->setTipPosition(mDataBaseEditorWidget->tipPosition() + QPoint(0, layerY - tipY));
}

void MainWindow::closeDataBaseEditorWidget()
{
    mDataBaseEditorWidget->hide();
    mChisel->currentRowDataModel()->loadTable();
    mChisel->pickedRowDataModel()->loadTable();
    mCurrentPaletteValueWidget->updateTableRow();
    mCurrentPaletteValueWidget->setRowViewVisible(true);    
    updateVisibleColumns();
}

void MainWindow::updateVisibleColumns()
{
    LOG("MainWindow::Change data visualization from Database Layer");
    mUi->Visualizer->makeCurrent();
    mChisel->updateVisibleColumns();
    
    mCurrentPaletteValueWidget->setRowViewVisible(true);
    mCurrentPaletteValueWidget->setRowViewColumnVisibility(mChisel->visibleColumnModel()->secondaryColumns());    
    mCurrentPaletteValueWidget->updatePaintingData();
    
    if(mUi->graphicViewPalette->palette() != mChisel->currentPalette())
        mUi->graphicViewPalette->setPalette(mChisel->currentPalette());
    else
        mUi->graphicViewPalette->updatePaletteScene();    
    
    if(mChisel->visibleColumnModel()->primaryColumn() > 0)
    {
        mCurrentPaletteValueWidget->hide();
        if(mUi->actionMarkTool->isChecked())//if(mChisel->renderer()->isMarkModeActive())
            mUi->actionMarkTool->trigger();
        
        enableEditingTools(false);
    }
    else if(mChisel->visibleColumnModel()->primaryColumn() == 0)
    {
        if(mChisel->dataBaseTableDataModel()->rowCount() > 0)
            enableEditingTools(true);
    }
}


void MainWindow::processSQLStatement(const std::string& sqlStatement)
{
    mChisel->databaseManager()->beginTransaction();
    auto result = mChisel->databaseManager()->query(sqlStatement);
    mChisel->databaseManager()->commitTransaction();
}

void MainWindow::setPickedValue(double value)
{
    mUi->actionMarkTool->trigger();
    mCurrentPaletteValueWidget->setNewValue(value);    
    
    auto toolPos = mUi->Visualizer->mapFromGlobal(QCursor::pos());
    mChisel->renderer()->updateMarkToolPosition(toolPos.x(), toolPos.y());
}

void MainWindow::openChiselProject()
{
    int dialogValue = QMessageBox::Discard;
    if(mChisel->isSessionDirty())
        dialogValue = openDirtyChiselDialog();
    
    if(dialogValue == QMessageBox::Save)
        saveChiselProject();
    
    if(dialogValue != QMessageBox::Cancel)
    {
        auto filepath = QFileDialog::getOpenFileName(this, tr("Open CHISel Project File"), QApplication::applicationDirPath(), tr("Chisel files (*.chl)"));
        QFileInfo file(filepath);
/*        auto projectPath = QFileDialog::getExistingDirectory(this, tr("Open CHISel Project"), QCoreApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
        QDir projectDir(projectPath);
        auto temp = projectDir.dirName() + ".chl";
        QFileInfo file(projectDir, projectDir.dirName() + ".chl");   */ 
        
        if(file.exists())
        {
            mUi->Visualizer->makeCurrent();
            mChisel->loadChiselFile(file.baseName().toStdString(), file.absolutePath().toStdString() + "/");
            mUi->Visualizer->update();
            
            QSignalBlocker blocker(mUi->activeLayerTreeView->selectionModel());
            mUi->activeLayerTreeView->setCurrentIndex(mChisel->activeLayerModel()->index(0, 0));
            blocker.unblock();    
            
            if(mOpacityEditorWidget != nullptr && mOpacityEditorWidget->isVisible())
                mOpacityEditorWidget->hide();  
            
            // TODO: Merge both if(mChisel->hasLayers)
            
            if(mChisel->hasLayers())
            {
                if(mChisel->currentLayer()->discrete())
                {
                    mCurrentPaletteValueWidget->setIntValidator(static_cast<int>(mChisel->currentPalette()->minValue()), static_cast<int>(mChisel->currentPalette()->maxValue()));
                    mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Int);
                    
                    if(mChisel->currentLayer()->registerType())
                    {
                        auto columnVisibility = dynamic_cast<const RegisterLayer *>(mChisel->currentLayer())->secondaryColumns();

                        mCurrentPaletteValueWidget->setRowViewVisible(true);
                        mCurrentPaletteValueWidget->setRowViewColumnVisibility(columnVisibility);
                        mCurrentPaletteValueWidget->updatePaintingData();

                        //mDataBaseEditorWidget->setHiddenColumns(hidden);
                    }
                    else
                    {
                        mDataBaseEditorWidget->hide();
                        mCurrentPaletteValueWidget->setRowViewVisible(false);            
                    }
                }
                else
                {
                    mCurrentPaletteValueWidget->setDoubleValidator(mChisel->currentPalette()->minValue(), mChisel->currentPalette()->maxValue());
                    mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Double);    
                    
                    mDataBaseEditorWidget->hide();
                    mCurrentPaletteValueWidget->setRowViewVisible(false);                    
                }
                
                mCurrentPaletteValueWidget->updatePaintingData();
                updatePaletteInformation();                
            }
                                
            if(mChisel->hasLayers())
                setState(State::LayerLoaded);
            else
                setState(State::ModelLoaded);
        }
    }
}

void MainWindow::import3DModel()
{
    auto filepath = QFileDialog::getOpenFileName(this, tr("Import 3D Model"), QApplication::applicationDirPath(), tr("3D Models (*.obj *.ply *.fbx)"));
    QFileInfo file(filepath);
    
    if(file.exists())
    {
        using clock_type = typename std::conditional< std::chrono::high_resolution_clock::is_steady,
                                                  std::chrono::high_resolution_clock,
                                                  std::chrono::steady_clock >::type ;
        mUi->Visualizer->makeCurrent();
        auto t_start = clock_type::now();
        mChisel->import3DModel(file.baseName().toStdString(), file.suffix().toStdString(), file.absolutePath().toStdString() + "/");
        auto t_end = clock_type::now();
        std::cout << "Import time: " << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " milliseconds" << std::endl;
        mUi->Visualizer->update();
        
        setState(State::ModelLoaded);
    }    
}

void MainWindow::exportChiselModel()
{
    FBXExporterDialog dialog(mChisel.get(), this);
    
    dialog.exec();
//     auto filepath = QFileDialog::getSaveFileName(this, tr("Save Chisel Model As"), QApplication::applicationDirPath() + "/" + QString::fromStdString(mChisel->chiselName()), tr("FBX (*.fbx)"));
//     
//     if(filepath.size())
//     {
//         QFileInfo file(filepath);
//         mChisel->exportChiselModel(file.absoluteFilePath().toStdString(), file.suffix().toStdString(), mUi->activeLayerTreeView->currentIndex().row());
//     }
}

void MainWindow::saveChiselProject()
{
    mUi->Visualizer->makeCurrent();
    
    if(mChisel->isChiselPathValid())
        mChisel->saveChiselProject();
    else
        saveChiselProjectAs();
}

void MainWindow::saveChiselProjectAs()
{
    //auto filepath = QFileDialog::getSaveFileName(this, tr("Save Chisel File As"), QApplication::applicationDirPath() + "/" + QString::fromStdString(mChisel->chiselName()), tr("Chisel (*.chl)"));
    
    QFileDialog dialog(this, tr("Save Chisel Project As"), QApplication::applicationDirPath() + "/" + QString::fromStdString(mChisel->chiselName()));    
    //dialog.setFileMode(QFileDialog::Directory); // Can't save in this mode
    dialog.setAcceptMode(QFileDialog::AcceptSave);    
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    
    if(dialog.exec())
    {
        QFileInfo file(dialog.selectedFiles().first());
        mChisel->saveChiselProject(file.baseName().toStdString(), file.absoluteFilePath().toStdString() + "/");
    }
}

void MainWindow::closeChisel()
{

}


int MainWindow::openDirtyChiselDialog()
{
    QMessageBox msgBox;
    msgBox.setText("The current session has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    return msgBox.exec();
}

void MainWindow::createLayer(QString name, Layer::Type type, std::pair< int, int > resolution, int paletteIndex)
{
    mUi->Visualizer->makeCurrent();
    mChisel->createLayer(name.toStdString(), type, resolution);        
    mUi->Visualizer->setFocus();

    mUi->Visualizer->hideReadValueDialog();

    QSignalBlocker blocker(mUi->activeLayerTreeView->selectionModel());
    mUi->activeLayerTreeView->setCurrentIndex(mChisel->activeLayerModel()->index(0, 0));
    blocker.unblock();

    if(paletteIndex >= 0)
        mUi->paletteListView->setCurrentIndex(mChisel->paletteListModel()->index(paletteIndex, 0));
    
    mUi->graphicViewPalette->updatePaletteScene();
    
    if(mChisel->currentLayer()->discrete())
    {
        mCurrentPaletteValueWidget->setIntValidator(static_cast<int>(mChisel->currentPalette()->minValue()), static_cast<int>(mChisel->currentPalette()->maxValue()));
        mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Int);
        
        if(mChisel->currentLayer()->registerType())
        {
            auto columnVisibility = dynamic_cast<const RegisterLayer *>(mChisel->currentLayer())->secondaryColumns();

            auto regLayer = dynamic_cast<const RegisterLayer*>(mChisel->currentLayer());
            if(regLayer->hasResources())
                mDataBaseEditorWidget->setResourceDirectory(regLayer->name());
            else
                mDataBaseEditorWidget->clearResourceDirectorySelection();
            
            mCurrentPaletteValueWidget->setRowViewVisible(true);               
            mCurrentPaletteValueWidget->setRowViewColumnVisibility(columnVisibility);
            mCurrentPaletteValueWidget->updatePaintingData();
        }
        else
        {
            mDataBaseEditorWidget->hide();
            mCurrentPaletteValueWidget->setRowViewVisible(false);            
        }
    }
    else
    {
        mCurrentPaletteValueWidget->setDoubleValidator(mChisel->currentPalette()->minValue(), mChisel->currentPalette()->maxValue());
        mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Double);    
        
        mDataBaseEditorWidget->hide();
        mCurrentPaletteValueWidget->setRowViewVisible(false);                    
    }
    
    mCurrentPaletteValueWidget->updatePaintingData();
    
    if(mOpacityEditorWidget != nullptr && mOpacityEditorWidget->isVisible())
        mOpacityEditorWidget->hide();
    
    if(mChisel->currentLayer()->registerType())
    {   
        mDataBaseEditorWidget->setLayer(0);
        mDataBaseEditorWidget->show();
        
        auto index = mUi->activeLayerTreeView->model()->index(0, 0);
        auto posY = mUi->activeLayerTreeView->mapTo(mUi->centralwidget, mUi->activeLayerTreeView->visualRect(index).center()).y() + 2;
        auto posX = mUi->Visualizer->pos().x();
        mDataBaseEditorWidget->move(QPoint(posX, posY) - QPoint(0, mDataBaseEditorWidget->tipPosition().y()));
        
        connect(mChisel->dataBaseTableSchemaModel(), &DataBaseTableSchemaModel::tableSchemaCreated, this, &MainWindow::processSQLStatement);
    }
    
    setState(State::LayerCreated);
    //mUi->actionMarkTool->trigger();
}

void MainWindow::openLayerOperationWidget()
{
    if(mDataBaseEditorWidget->isVisible()) mDataBaseEditorWidget->hide();
    if(mOpacityEditorWidget->isVisible()) mOpacityEditorWidget->hide();

    mLayerOperationWidget->show();
    
    auto index = mUi->activeLayerTreeView->model()->index(0, 0);
    auto posY = mUi->activeLayerTreeView->mapTo(mUi->centralwidget, mUi->activeLayerTreeView->visualRect(index).topRight()).y() + 2;
    auto posX = mUi->Visualizer->pos().x();
    mLayerOperationWidget->move({posX, posY - mLayerOperationWidget->height() * 3/4});      
    
    if(!mLayerOperationWidget->isTipPositionFixed())
        mLayerOperationWidget->setFixedTipPosition(true);
    
    mLayerOperationWidget->setEditorFocus();
}

void MainWindow::duplicateLayer()
{
    mUi->Visualizer->makeCurrent();
    
    mChisel->duplicateLayer(mUi->activeLayerTreeView->currentIndex().row());

    mUi->Visualizer->update();    
}

void MainWindow::unloadLayer()
{
    if(mUi->activeLayerTreeView->currentIndex().isValid())
    {
        mUi->Visualizer->makeCurrent();
    
        if(mUi->layerToolBox->currentIndex() == 1)
            mChisel->unloadLayer(mUi->activeLayerTreeView->currentIndex().row());
        else
        {
            for(const auto& name : mUi->diskLayerTreeView->selectedLayerNames())
                mChisel->unloadLayer(name);
        }
        
        mUi->Visualizer->update();
    }
    
    if(!mChisel->hasLayers())
        setState(State::ModelLoaded);    
}

void MainWindow::eraseLayer()
{
    mUi->Visualizer->makeCurrent();
    
    if(mUi->layerToolBox->currentIndex() == 1)    
        mChisel->eraseLayer(mUi->activeLayerTreeView->currentIndex().row());
    else
    {
       for(const auto& name : mUi->diskLayerTreeView->selectedLayerNames())
           mChisel->eraseLayer(name);
    }
        
    mUi->Visualizer->update();

    if(!mChisel->hasLayers())
        setState(State::ModelLoaded);    
}

void MainWindow::renameLayer(const QString& newName)
{
    mChisel->renameLayer(mUi->activeLayerTreeView->currentIndex().row(), newName.toStdString());
}


void MainWindow::triggerRenameLayer()
{
    mUi->activeLayerTreeView->edit(mUi->activeLayerTreeView->currentIndex());
}


void MainWindow::computeExpression(const QString expression)
{
    mUi->Visualizer->makeCurrent();
    
    auto errorMessages = mChisel->computeExpression(expression.toStdString());
    
    if(errorMessages.size())
    {
        mLayerOperationWidget->setEditorText(errorMessages[0]);
        mLayerOperationWidget->setErrorLogText(errorMessages[1]);
    }
    else
        mLayerOperationWidget->hideWidget();
    
    mUi->Visualizer->update();    
}

void MainWindow::loadLayersSlot()
{
    mUi->Visualizer->makeCurrent();
    
    if(mUi->layerToolBox->currentIndex() == 1)    
    {
        
    }
    else
    {
        auto selectedLayerNames = mUi->diskLayerTreeView->selectedLayerNames();
        if(selectedLayerNames.size()) loadLayers(selectedLayerNames); 
    }
}


void MainWindow::loadLayers(const std::vector<std::string>& layerNames)
{
    mUi->Visualizer->makeCurrent();
    for(const auto& layerName: layerNames)
        mChisel->loadLayer(layerName);
    mUi->Visualizer->setFocus();
    
    // Update palette information
    
    QSignalBlocker blocker(mUi->activeLayerTreeView->selectionModel());
    mUi->activeLayerTreeView->setCurrentIndex(mChisel->activeLayerModel()->index(0, 0));
    blocker.unblock();

//     QSignalBlocker pblocker(mUi->paletteListView->selectionModel());
//     if(mChisel->currentPalette()->isDirty())
//         mUi->paletteListView->setCurrentIndex(QModelIndex());
//     else    
//         mUi->paletteListView->setCurrentIndex(mChisel->paletteListModel()->index(static_cast<int>(mChisel->currentPaletteIndex()), 0));
//     pblocker.unblock();
    
    mUi->graphicViewPalette->updatePaletteScene();
    
    if(mChisel->currentLayer()->discrete())
    {
        mCurrentPaletteValueWidget->setIntValidator(static_cast<int>(mChisel->currentPalette()->minValue()), static_cast<int>(mChisel->currentPalette()->maxValue()));
        mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Int);
        
        if(mChisel->currentLayer()->registerType())
        {
            auto columnVisibility = dynamic_cast<const RegisterLayer *>(mChisel->currentLayer())->secondaryColumns();

            mCurrentPaletteValueWidget->setRowViewVisible(true);
            mCurrentPaletteValueWidget->setRowViewColumnVisibility(columnVisibility);
            mCurrentPaletteValueWidget->updatePaintingData();

            //mDataBaseEditorWidget->setHiddenColumns(hidden);
        }
        else
        {
            mCurrentPaletteValueWidget->setRowViewVisible(false);
            mDataBaseEditorWidget->hide();            
        }
    }
    else
    {
        mCurrentPaletteValueWidget->setDoubleValidator(mChisel->currentPalette()->minValue(), mChisel->currentPalette()->maxValue());
        mUi->paletteEditor->paletteTableView()->lineDelegate()->setValidatorType(ValidatedDelegate::ValidatorType::Double);
        
        mCurrentPaletteValueWidget->setRowViewVisible(false);
        mDataBaseEditorWidget->hide();        
    }
    
    mCurrentPaletteValueWidget->updatePaintingData();
    
    if(mOpacityEditorWidget != nullptr && mOpacityEditorWidget->isVisible())
        mOpacityEditorWidget->hide();
                
    setState(State::LayerLoaded);
}


void MainWindow::exportLayerAsImage()
{
    auto defaultFileName = mChisel->currentLayer()->name() + ".png";
    auto filepath = QFileDialog::getSaveFileName(this, tr("Export Layer"), QApplication::applicationDirPath() + QString::fromStdString(defaultFileName), tr("Images (*.png)"));
    
    mUi->Visualizer->makeCurrent();    
    mChisel->exportLayerAsImage(filepath.toStdString(), mUi->activeLayerTreeView->currentIndex().row());
}

void MainWindow::updateLayerResourceFiles()
{
    mChisel->dataBaseTableDataModel()->setResourceFiles(mChisel->resourceManager()->layerResourceFiles(mChisel->currentLayer()->name()));
    mChisel->currentRowDataModel()->setResourceFiles(mChisel->resourceManager()->layerResourceFiles(mChisel->currentLayer()->name()));
}

void MainWindow::saveTableSchemaToFile()
{
    auto index = mUi->activeLayerTreeView->currentIndex();
    
    if(index.isValid())
        mChisel->saveLayer(index.row());
}

bool MainWindow::enableDatabaseLayerEditing()
{
    if(mChisel->dataBaseTableDataModel()->rowCount() > 0)
    {
        if(mChisel->currentLayer()->isVisible() && !mUi->actionMarkTool->isEnabled() && mChisel->visibleColumnModel()->primaryColumn() == 0)
        {
            mUi->actionMarkTool->setEnabled(true);
            mUi->actionEraserTool->setEnabled(true);
            mUi->actionPickerTool->setEnabled(true);
            return true;
        }
    }
    else if(mUi->actionMarkTool->isEnabled())
    {
        if(mUi->actionMarkTool->isChecked()) mUi->actionMarkTool->trigger();
        mUi->actionMarkTool->setEnabled(false);                
    }
    
    return false;
}

void MainWindow::createPalette()
{
//     mChisel->createPalette("");
    
//     mUi->toolBoxPalette->setItemText(1, QString::fromStdString(mChisel->currentPalette()->name()));    
//     mUi->paletteListView->editNewPalette();
    mChisel->createPalette("");

    mUi->paletteButtonWidget->hide();
    mUi->paletteListView->hide();
    mUi->applyPaletteToLayerButton->hide();
    mUi->paletteCreator->setState(PaletteCreatorWidget::State::Create);
    mUi->paletteCreator->show();
}

void MainWindow::editPalette()
{
    auto index = mUi->paletteListView->currentIndex();
    
    if(index.isValid())
    {
        mChisel->duplicatePalette(index.row(), false);
        auto paletteCount = mUi->paletteListView->model()->rowCount();    
        auto copyIndex = mUi->paletteListView->model()->index(paletteCount - 1, 0);
        mChisel->editPalette(copyIndex.row());

        mUi->paletteButtonWidget->hide();
        mUi->paletteListView->hide();
        mUi->applyPaletteToLayerButton->hide();
        mUi->paletteCreator->setState(PaletteCreatorWidget::State::Edit);
        mUi->paletteCreator->show();
    }    
}

void MainWindow::importPalette()
{
    QFileDialog importDialog(this, tr("Import Palettes"), QApplication::applicationDirPath(), tr("Palette files (*.plt)"));
    importDialog.setLabelText(QFileDialog::Accept, "Import");
    
    if(mUi->toolBoxPalette->currentIndex() == 0)
    {
        importDialog.setFileMode(QFileDialog::ExistingFiles);
            
        if(importDialog.exec())
        {
            bool cancelOperation = false;

            for(int i = 0; i < importDialog.selectedFiles().size() && !cancelOperation; ++i)
            {   
                auto selectedPalettePath =  importDialog.selectedFiles().at(i);
                QFileInfo selectedPalette(selectedPalettePath);
                
                auto newPalettePath = QString(mChisel->palettesPath().c_str()) + selectedPalette.fileName();            
                QFileInfo newPalette(newPalettePath);
                
                QDir dir(QString(mChisel->palettesPath().c_str()));            
                
                if(!dir.exists(newPalette.fileName()))
                {
                    QFile::copy(selectedPalettePath, newPalettePath);
                    mChisel->importPalette(selectedPalette.baseName().toStdString(), mChisel->palettesPath());
                }
                else
                {
                    auto fileNames = dir.entryList(QStringList("*.plt"), QDir::Files);
                    
                    OverwriteDialog dialog(selectedPalette, newPalette, fileNames, "palette");
                    
                    if (dialog.exec())
                    {
                        if (dialog.renamed())
                        {
                            auto newPalettePath = QString(mChisel->palettesPath().c_str()) + dialog.newName();
                            QFile::copy(selectedPalettePath, newPalettePath);
                            QFileInfo renamed(newPalettePath);
                            mChisel->importPalette(renamed.baseName().toStdString(), mChisel->palettesPath());
                            
                        }
                        else if (dialog.overwritten())
                        {
                            mChisel->erasePalette(mChisel->palette(selectedPalette.baseName().toStdString())->index());
                            QFile::copy(selectedPalettePath, newPalettePath);                        
                            mChisel->importPalette(selectedPalette.baseName().toStdString(), mChisel->palettesPath());
                        }
                        
                    }
                    else
                        cancelOperation = true;            
                }
            }
        }
    }
    else
    {
        importDialog.setFileMode(QFileDialog::ExistingFile);
        
        if(importDialog.exec())
        {
            QFileInfo file(importDialog.selectedFiles().at(0));            
            mChisel->importPaletteToLayer(file.baseName().toStdString(), file.absolutePath().toStdString() + "/");
            updatePaletteInformation();            
        }
    }
}

void MainWindow::exportPalette()
{
    auto index = mUi->paletteListView->currentIndex();
    
    if(index.isValid())
    {
        auto filepath = QFileDialog::getSaveFileName(this, tr("Export Palette As"), QApplication::applicationDirPath(), tr("Palette files (*.plt)"));

        if(filepath.size() > 0)
        {
            QFileInfo file(filepath);
            mChisel->exportPalette(mUi->paletteListView->currentIndex().row(), file.baseName().toStdString(), file.absolutePath().toStdString() + "/");
        }
    }
}

void MainWindow::duplicatePalette()
{
    auto index = mUi->paletteListView->currentIndex();
    
    if(index.isValid())
    {
        mChisel->duplicatePalette(index.row());
        
        auto paletteCount = mUi->paletteListView->model()->rowCount();
        auto newIndex = mUi->paletteListView->model()->index(paletteCount - 1, 0);
        mChisel->savePalette(newIndex.row());
        
        mUi->paletteListView->setCurrentIndex(newIndex);
    }
}

void MainWindow::erasePalette()
{
    auto index = mUi->paletteListView->currentIndex();
    
    if(index.isValid())
        mChisel->erasePalette(index.row());    
}

void MainWindow::addPaletteToCollection()
{
    mChisel->addPaletteToCollection();
}

void MainWindow::applySelectedPaletteToLayer()
{
    LOG("MainWindow::Apply Selected Palette To Layer ");
    
    if(mUi->paletteListView->currentIndex().isValid())
    {
        mUi->Visualizer->makeCurrent();
        mChisel->setCurrentPalette(mUi->paletteListView->currentIndex().row());
        mUi->toolBoxPalette->setCurrentIndex(1);
    }
}

void MainWindow::editLightingParameters()
{
    mLightingDialog = new LightingDialog(this);
    mLightingDialog->setAttribute(Qt::WA_DeleteOnClose);
    mLightingDialog->setSpecularComponentState(mChisel->specularColor().x != 0.0);
    mLightingDialog->setSpecularColor(mChisel->specularColor().x);
    mLightingDialog->setSpecularPower(mChisel->specularPower());
    
    connect(mLightingDialog, &LightingDialog::specularColorEdited, this, &MainWindow::setSpecularColor);
    connect(mLightingDialog, &LightingDialog::specularPowerEdited, this, &MainWindow::setSpecularPower);

    mLightingDialog->move(mUi->Visualizer->mapToGlobal(QPoint(6, 40)));
    mLightingDialog->show();
}

void MainWindow::setSpecularColor(float value)
{
    mUi->Visualizer->makeCurrent();
    mChisel->setSpecularColor(glm::vec3(value));
    mUi->Visualizer->update();
}

void MainWindow::setSpecularPower(float value)
{
    LOG("MainWindow::specularPower: ", value);
    mUi->Visualizer->makeCurrent();
    mChisel->setSpecularPower(value);
    mUi->Visualizer->update();
}

void MainWindow::toggleVertexColorVisibility(bool checked)
{
    mUi->actionDefaultModelColor->setEnabled(!checked);
    
    mUi->Visualizer->makeCurrent();
    mChisel->toggleVertexColorVisibility(checked);
    mUi->Visualizer->update();
}

void MainWindow::selectDefaultModelColor()
{
    auto currentColor = mChisel->defaultModelColor();
        
    auto newColor = QColorDialog::getColor(QColor::fromRgbF(currentColor.x, currentColor.y, currentColor.z, currentColor.w), this->parentWidget(), "Select Default Model Color:", QColorDialog::ShowAlphaChannel);
        
    if(newColor.isValid())
    {
        mUi->Visualizer->makeCurrent();        
        mChisel->setDefaultModelColor({newColor.redF(), newColor.greenF(), newColor.blueF(), newColor.alphaF()});
        mUi->Visualizer->update();
    }
}

void MainWindow::selectBackgroundColor()
{
	auto currentColor = mChisel->backgroundColor();

	auto newColor = QColorDialog::getColor(QColor::fromRgbF(currentColor.x, currentColor.y, currentColor.z, currentColor.w), this->parentWidget(), "Select Color:", QColorDialog::ShowAlphaChannel);

	if (newColor.isValid())
	{
		mUi->Visualizer->makeCurrent();
		mChisel->setBackgroundColor({ newColor.redF(), newColor.greenF(), newColor.blueF(), newColor.alphaF() });
		mUi->Visualizer->update();
	}
}

void MainWindow::alignCameraToModel()
{
	mChisel->alignMainCameraToModel();
	mUi->Visualizer->update();
}

void MainWindow::updateMarkToolRadius(int sliderValue)
{
    auto rendererWidgetSize = mUi->Visualizer->size();
    auto maxRadius = std::min(rendererWidgetSize.width(), rendererWidgetSize.height())/2;
    
    auto x = 2.5;
    auto y = maxRadius/5;
    auto z = maxRadius;
    
    auto yx = y - x;
    auto x2yz = x - 2 * y + z;
    
    auto a = (x * z  - y * y) / x2yz;
    auto b = (yx * yx) / x2yz;
    auto c = 2 * std::log((z - y) / yx);
    
    auto value = a + b * std::exp(c * sliderValue / (1.0 * mMarkToolRadiusSlider->maximum()));
    
    mChisel->renderer()->updateMarkToolRadius(value); 
}

void MainWindow::updateMarkToolSlider(float radius)
{
    auto rendererWidgetSize = mUi->Visualizer->size();
    auto maxRadius = std::min(rendererWidgetSize.width(), rendererWidgetSize.height())/2;
    
    auto x = 2.5;
    auto y = maxRadius/5;
    auto z = maxRadius;
    
    auto yx = y - x;
    auto x2yz = x - 2 * y + z;
    
    auto a = (x * z  - y * y) / x2yz;
    auto b = (yx * yx) / x2yz;
    auto c = 2 * std::log((z - y) / yx);

    auto sliderValue = std::log((radius - a) / b) / c * mMarkToolRadiusSlider->maximum();
    
    mMarkToolRadiusSlider->setValue(sliderValue);
}

void MainWindow::incrementMarkToolSlider(int increment)
{
    mMarkToolRadiusSlider->setValue(mMarkToolRadiusSlider->value() + increment);
    mUi->Visualizer->update();
}

void MainWindow::clearSlice()
{
    mUi->actionSliceModelTool->trigger();
    
    mChisel->renderer()->clearSlice();
    mUi->Visualizer->update();
}

// *** Protected *** //
void MainWindow::createActions()
{
    connect(mUi->actionOpen, &QAction::triggered, this, &MainWindow::openChiselProject);
    connect(mUi->actionImport, &QAction::triggered, this, &MainWindow::import3DModel);
    connect(mUi->actionExportModel, &QAction::triggered, this, &MainWindow::exportChiselModel);
    connect(mUi->actionSave, &QAction::triggered, this, &MainWindow::saveChiselProject);
    connect(mUi->actionSaveAs, &QAction::triggered, this, &MainWindow::saveChiselProjectAs);
    connect(mUi->actionExit, &QAction::triggered, this, &MainWindow::close);
    
    connect(mUi->actionEditLightingParameters, &QAction::triggered, this, &MainWindow::editLightingParameters);
    connect(mUi->actionShowVertexColor, &QAction::toggled, this, &MainWindow::toggleVertexColorVisibility);
    connect(mUi->actionDefaultModelColor, &QAction::triggered, this, &MainWindow::selectDefaultModelColor);
	connect(mUi->actionBackgroundColor, &QAction::triggered, this, &MainWindow::selectBackgroundColor);
	connect(mUi->actionAlignCameraToModel, &QAction::triggered, this, &MainWindow::alignCameraToModel);

    connect(mUi->actionExportToTexture, &QAction::triggered, this, &MainWindow::exportLayerAsImage);
    connect(mUi->actionRenameLayer, &QAction::triggered, this, &MainWindow::triggerRenameLayer);
    connect(mUi->actionDuplicateLayer, &QAction::triggered, this, &MainWindow::duplicateLayer);
    connect(mUi->actionLoadLayer, &QAction::triggered, this, &MainWindow::loadLayersSlot);
    connect(mUi->actionUnloadLayer, &QAction::triggered, this, &MainWindow::unloadLayer);
    connect(mUi->actionEraseLayer, &QAction::triggered, this, &MainWindow::eraseLayer);
    connect(mUi->actionOperateLayer, &QAction::triggered, this, &MainWindow::openLayerOperationWidget);
    
    connect(mUi->actionCreatePalette, &QAction::triggered, this, &MainWindow::createPalette);
    connect(mUi->actionEditPalette, &QAction::triggered, this, &MainWindow::editPalette);
    connect(mUi->actionDuplicatePalette, &QAction::triggered, this, &MainWindow::duplicatePalette);
    connect(mUi->actionErasePalette, &QAction::triggered, this, &MainWindow::erasePalette);
    connect(mUi->actionImportPalette, &QAction::triggered, this, &MainWindow::importPalette);
    connect(mUi->actionExportPalette, &QAction::triggered, this, &MainWindow::exportPalette);
    connect(mUi->actionAddPaletteToCollection, &QAction::triggered, this, &MainWindow::addPaletteToCollection);

    connect(mUi->actionCreateHistogram, &QAction::triggered, this, &MainWindow::createHistogram);
        
/*    mSignalMapper = new QSignalMapper(this);
    mSignalMapper->setMapping(mUi->actionSilhouette, Technique::Silhouette);
    mSignalMapper->setMapping(mUi->actionTransform, Technique::Transform);
    mSignalMapper->setMapping(mUi->actionHatching, Technique::Hatching);
    mSignalMapper->setMapping(mUi->actionBlending, Technique::Blending);

    connect(mUi->actionSilhouette, &QAction::triggered, mSignalMapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::map));
    connect(mUi->actionTransform, &QAction::triggered, mSignalMapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::map));
    connect(mUi->actionHatching, &QAction::triggered, mSignalMapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::map));
    connect(mUi->actionBlending, &QAction::triggered, mSignalMapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::map));
    
    connect(mUi->actionSilhouette, SIGNAL(triggered(bool)), mSignalMapper, SLOT(map()));
    connect(mUi->actionTransform, SIGNAL(triggered(bool)), mSignalMapper, SLOT(map()));
    connect(mUi->actionHatching, SIGNAL(triggered(bool)), mSignalMapper, SLOT(map()));
    connect(mUi->actionBlending, SIGNAL(triggered(bool)), mSignalMapper, SLOT(map()));
    
    connect(mSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), mComposer, &TechniqueComposer::insertTechnique);
    connect(mSignalMapper, SIGNAL(mapped(int)), mComposer, SLOT(insertTechnique(int)));*/
}

void MainWindow::createToolBar()
{
    mUi->createLayerButton->setDefaultAction(mUi->actionCreateLayer);
    mUi->duplicateLayerButton->setDefaultAction(mUi->actionDuplicateLayer);
    mUi->unloadLayerButton->setDefaultAction(mUi->actionUnloadLayer);
    
    mUi->loadLayerButton->setDefaultAction(mUi->actionLoadLayer);
    mUi->unloadLayerButtonDisk->setDefaultAction(mUi->actionUnloadLayer);
    mUi->eraseLayerButtonDisk->setDefaultAction(mUi->actionEraseLayer);
    
    mUi->createPaletteButton->setDefaultAction(mUi->actionCreatePalette);
    mUi->editPaletteButton->setDefaultAction(mUi->actionEditPalette);
    mUi->duplicatePaletteButton->setDefaultAction(mUi->actionDuplicatePalette);
    mUi->erasePaletteButton->setDefaultAction(mUi->actionErasePalette);
    mUi->importPaletteButton->setDefaultAction(mUi->actionImportPalette);
    mUi->exportPaletteButton->setDefaultAction(mUi->actionExportPalette);
    
    mUi->paletteEditor->addToCollectionButton()->setDefaultAction(mUi->actionAddPaletteToCollection);
    mUi->paletteEditor->importButton()->setDefaultAction(mUi->actionImportPalette);
    
    QWidget* spacer = new QWidget(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(spacer);
    auto iconWidth = mUi->toolBar->iconSize().width();
    horizontalLayout->insertSpacing(0, mUi->Visualizer->pos().x() + 640 - 2 * iconWidth - 6 * iconWidth / 2 - 50);
    
    mUi->toolBar->addWidget(spacer);
    
    mEditingToolGroup = std::make_unique<QActionGroup>(this);
    mEditingToolGroup->addAction(mUi->actionMarkTool);
    mEditingToolGroup->addAction(mUi->actionEraserTool);
    mEditingToolGroup->addAction(mUi->actionPickerTool);
    mEditingToolGroup->addAction(mUi->actionValuePickerTool);
    mEditingToolGroup->addAction(mUi->actionSliceModelTool);    
    mEditingToolGroup->addAction(mUi->actionOperateLayer);
    mUi->toolBar->addActions(mEditingToolGroup->actions());

    connect(mEditingToolGroup.get(), &QActionGroup::triggered, this, &MainWindow::updateEditingActionGroupState);
    connect(mEditingToolGroup.get(), &QActionGroup::triggered, this, &MainWindow::changeEditingMode);
    
    mMarkToolRadiusSlider = new QSlider();
    mMarkToolRadiusSlider->setMaximumWidth(250);
    mMarkToolRadiusSlider->setMaximum(1000);
    mMarkToolRadiusSlider->setMinimum(0);
    mMarkToolRadiusSlider->setOrientation(Qt::Horizontal);
    connect(mMarkToolRadiusSlider, &QSlider::valueChanged, this, &MainWindow::updateMarkToolRadius);
    connect(mUi->Visualizer, &OpenGLWidget::markToolRadiusChanged, this, &MainWindow::incrementMarkToolSlider);    

    QWidget* spacer2 = new QWidget(this);
    QHBoxLayout* horizontalLayout2 = new QHBoxLayout(spacer2);
    horizontalLayout2->insertSpacing(0, iconWidth);        
    mUi->toolBar->addWidget(spacer2);
    
    mSeparatorAction = mUi->toolBar->addSeparator();
    mSeparatorAction->setVisible(false);
    
    QWidget* spacer3 = new QWidget(this);
    QHBoxLayout* spacerLayout3 = new QHBoxLayout(spacer3);
    spacerLayout3->insertSpacing(0, iconWidth);        
    mUi->toolBar->addWidget(spacer3);
        
    mMarkToolRadiusSliderAction = mUi->toolBar->addWidget(mMarkToolRadiusSlider);
    mMarkToolRadiusSliderAction->setVisible(false);

    mClearSlicePlaneButton = new QPushButton("Clear", this);
    mClearSlicePlaneButton->setObjectName("ClearSlicePlaneButton");
    mClearSlicePlaneButton->setMaximumSize(80, 35);
    connect(mClearSlicePlaneButton, &QPushButton::released, this, &MainWindow::clearSlice);    
    mClearSlicePlaneButtonAction = mUi->toolBar->addWidget(mClearSlicePlaneButton);
    mClearSlicePlaneButtonAction->setVisible(false);
//     auto newSlider = new QSlider();
//     newSlider->setMaximumWidth(300);
//     newSlider->setOrientation(Qt::Horizontal);
//     mUi->toolBar->addWidget(newSlider);
}

void MainWindow::createMenus()
{
    QMenu* activeLayerMenu = new QMenu(mUi->activeLayerTreeView);
    activeLayerMenu->addAction(mUi->actionCreateLayer);
    activeLayerMenu->addAction(mUi->actionExportToTexture);
    activeLayerMenu->addSeparator();
    activeLayerMenu->addAction(mUi->actionRenameLayer);
    activeLayerMenu->addAction(mUi->actionDuplicateLayer);
    activeLayerMenu->addAction(mUi->actionUnloadLayer);
    activeLayerMenu->addAction(mUi->actionEraseLayer);
    activeLayerMenu->addSeparator();
    activeLayerMenu->addAction(mUi->actionOperateLayer);
    
    QMenu* diskLayerMenu = new QMenu(mUi->diskLayerTreeView);
    
    mUi->activeLayerTreeView->setContextMenu(activeLayerMenu);
}


// *** Private Methods *** //

void MainWindow::enableEditingTools(bool enable)
{
    if(!enable)
    {
        if(mUi->actionMarkTool->isChecked()) mUi->actionMarkTool->trigger();
        if(mUi->actionEraserTool->isChecked()) mUi->actionEraserTool->trigger();
        if(mUi->actionPickerTool->isChecked()) mUi->actionPickerTool->trigger();
    }
    
    mUi->actionMarkTool->setEnabled(enable);
    mUi->actionEraserTool->setEnabled(enable);
    mUi->actionPickerTool->setEnabled(enable);
}

void MainWindow::setState(MainWindow::State state)
{
    mLastState = mState;
    mState = state;
    
    switch(mState)
    {
        case State::Idle:
        {
            mUi->actionCreateLayer->setDisabled(true);
            mUi->actionExportToTexture->setDisabled(true);
            mUi->actionLoadLayer->setDisabled(true);
            mUi->actionDuplicateLayer->setDisabled(true);
            mUi->actionUnloadLayer->setDisabled(true);
            mUi->actionEraseLayer->setDisabled(true);
            mUi->actionRenameLayer->setDisabled(true);
            mUi->actionOperateLayer->setDisabled(true);
            
            mUi->actionSave->setDisabled(true);
            mUi->actionSaveAs->setDisabled(true);
            mUi->actionExportModel->setDisabled(true);
            
            mUi->actionMarkTool->setDisabled(true);
            mUi->actionEraserTool->setDisabled(true);
            mUi->actionPickerTool->setDisabled(true);
            mUi->actionValuePickerTool->setDisabled(true);
            mUi->actionSliceModelTool->setDisabled(true);
//             mEditingToolGroup->setDisabled(true);

            if(mSeparatorAction != nullptr)
            {
                mSeparatorAction->setVisible(false);
                mClearSlicePlaneButtonAction->setVisible(false);
                mMarkToolRadiusSliderAction->setVisible(false);
            }
            
            mUi->actionShowVertexColor->setDisabled(true);
            mUi->actionDefaultModelColor->setDisabled(true);
            mUi->actionAlignCameraToModel->setDisabled(true);

            mUi->actionCreateHistogram->setDisabled(true);
            
            mUi->palettePanel->hide();
            
            if(mLayerOperationWidget != nullptr) mLayerOperationWidget->hide();
            if(mOpacityEditorWidget != nullptr) mOpacityEditorWidget->hide();
            if(mDataBaseEditorWidget != nullptr) mDataBaseEditorWidget->hide();
            if(mCurrentPaletteValueWidget != nullptr) mCurrentPaletteValueWidget->hide();
            mUi->Visualizer->hideReadValueDialog();
            
            if (mChisel->renderer() != nullptr)
            {
                updatePaletteControlPointCoordinates();

                mUi->actionMarkTool->setChecked(false);
                mUi->actionEraserTool->setChecked(false);
                mUi->actionPickerTool->setChecked(false);
                mUi->actionValuePickerTool->setChecked(false);
                mUi->actionSliceModelTool->setChecked(false);

                mChisel->renderer()->setMarkMode(false);
                mChisel->renderer()->setEraseMode(false);
                mChisel->renderer()->setPickMode(false);
                mChisel->renderer()->setReadMode(false);
                mChisel->renderer()->setSliceMode(false);
            }
            
            break;            
        }
        case State::ModelLoaded:
        {
            mUi->actionCreateLayer->setEnabled(true);
            mUi->actionExportToTexture->setDisabled(true);
            mUi->actionDuplicateLayer->setDisabled(true);
            mUi->actionUnloadLayer->setDisabled(true);
            mUi->actionEraseLayer->setDisabled(true);
            mUi->actionRenameLayer->setDisabled(true);
            mUi->actionOperateLayer->setEnabled(true);

            mUi->actionMarkTool->setDisabled(true);
            mUi->actionEraserTool->setDisabled(true);
            mUi->actionPickerTool->setDisabled(true);
            mUi->actionValuePickerTool->setDisabled(true);
            mUi->actionSliceModelTool->setEnabled(true);
            
            mSeparatorAction->setVisible(false);
            mClearSlicePlaneButtonAction->setVisible(false);
            mMarkToolRadiusSliderAction->setVisible(false);           
                        
            mUi->actionSave->setEnabled(true);
            mUi->actionSaveAs->setEnabled(true);
            mUi->actionExportModel->setEnabled(true);
            
            mUi->actionShowVertexColor->setEnabled(true);
            mUi->actionDefaultModelColor->setEnabled(!mUi->actionShowVertexColor->isChecked());
            mUi->actionAlignCameraToModel->setEnabled(true);

            mUi->actionCreateHistogram->setDisabled(true);
                        
            mUi->palettePanel->hide();
            
            mLayerOperationWidget->hide();
            mOpacityEditorWidget->hide();
            mDataBaseEditorWidget->hide();            
            mCurrentPaletteValueWidget->hide();
            mUi->Visualizer->hideReadValueDialog();
            
            updatePaletteControlPointCoordinates();

            mUi->actionMarkTool->setChecked(false);
            mUi->actionEraserTool->setChecked(false);
            mUi->actionPickerTool->setChecked(false);
            mUi->actionValuePickerTool->setChecked(false);
            mUi->actionSliceModelTool->setChecked(false);

            mChisel->renderer()->setMarkMode(false);
            mChisel->renderer()->setEraseMode(false);
            mChisel->renderer()->setPickMode(false);
            mChisel->renderer()->setReadMode(false);
            mChisel->renderer()->setSliceMode(false);
            
            break;
        }        
        case State::CreateLayer:
        {            
            break;
        }
        case State::EditLayer:
        {
            break;
        }
        case State::LayerCreated:
        {
            mUi->layerToolBox->setCurrentIndex(1);
            mUi->Visualizer->setFocus();
            
            mUi->actionCreateLayer->setEnabled(true);
            mUi->actionExportToTexture->setEnabled(true);
            mUi->actionDuplicateLayer->setEnabled(true);
            mUi->actionLoadLayer->setEnabled(true);
            mUi->actionUnloadLayer->setEnabled(true);
            mUi->actionEraseLayer->setEnabled(true);
            mUi->actionOperateLayer->setEnabled(true);            
            mUi->actionRenameLayer->setEnabled(true);
            
            mUi->actionSave->setEnabled(true);
            mUi->actionSaveAs->setEnabled(true);
            mUi->actionExportModel->setEnabled(true);

            mUi->actionEraserTool->setEnabled(true);
            mUi->actionPickerTool->setEnabled(true);
            mUi->actionValuePickerTool->setEnabled(true);           
            mUi->actionSliceModelTool->setEnabled(true);
            
            if(!mChisel->currentLayer()->registerType())
            {
                mUi->actionMarkTool->setEnabled(true);                
                mUi->actionMarkTool->setChecked(true);
                mLastEditingToolAction = mUi->actionMarkTool;
                mChisel->renderer()->setMarkMode(true);
                mSeparatorAction->setVisible(true);
                mMarkToolRadiusSliderAction->setVisible(true);
            }
            else
                mUi->actionMarkTool->setEnabled(false);


            mUi->actionShowVertexColor->setEnabled(true);
            mUi->actionDefaultModelColor->setEnabled(!mUi->actionShowVertexColor->isChecked());
            mUi->actionAlignCameraToModel->setEnabled(true);

            mUi->actionCreateHistogram->setEnabled(true);

            mCurrentPaletteValueWidget->show();
            mUi->palettePanel->show();
            
            mUi->graphicViewPalette->updatePaletteScene();            
            updatePaletteValueWidgetCoordinates();
            updatePaletteControlPointCoordinates();            
        }
        case State::LayerLoaded:
        {
            mUi->diskLayerTreeView->clearSelection();            
            mUi->layerToolBox->setCurrentIndex(1);            
            mUi->activeLayerTreeView->setFocus();
            
            mUi->actionCreateLayer->setEnabled(true);             
            mUi->actionExportToTexture->setEnabled(true);
            mUi->actionDuplicateLayer->setEnabled(true);
            mUi->actionLoadLayer->setEnabled(true);
            mUi->actionUnloadLayer->setEnabled(true);
            mUi->actionEraseLayer->setEnabled(true);
            mUi->actionOperateLayer->setEnabled(true);            
            mUi->actionRenameLayer->setEnabled(true);
            
            mUi->actionSave->setEnabled(true);
            mUi->actionSaveAs->setEnabled(true);
            mUi->actionExportModel->setEnabled(true);
            

            mUi->actionEraserTool->setEnabled(true);
            mUi->actionPickerTool->setEnabled(true);
            mUi->actionValuePickerTool->setEnabled(true);
            mUi->actionSliceModelTool->setEnabled(true);
            
            if(!mChisel->currentLayer()->registerType())
                mUi->actionMarkTool->setEnabled(true);                
            else
                enableDatabaseLayerEditing();
            
            mUi->actionShowVertexColor->setEnabled(true);
            mUi->actionDefaultModelColor->setEnabled(!mUi->actionShowVertexColor->isChecked());
            mUi->actionAlignCameraToModel->setEnabled(true);

            mUi->actionCreateHistogram->setEnabled(true);
                        
            mUi->palettePanel->show();
            
            mUi->graphicViewPalette->updatePaletteScene();            
            updatePaletteValueWidgetCoordinates();
            updatePaletteControlPointCoordinates();
            
            break;
        }        
        case State::EditingToolsEnabled:
        {
            enableEditingTools(true);
            mState = mLastState;
            break;            
        }
        case State::EditingToolsDisabled:
        {
            enableEditingTools(false);
            mState = mLastState;
            break;
        }
    }
}


