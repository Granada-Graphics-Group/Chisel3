#include "databaseeditorwidget.h"
#include "databasetableschemaview.h"
#include "databasetableschemamodel.h"
#include "databasetabledataview.h"
#include "databasetabledatamodel.h"

#include "columnvisibilitywidget.h"
#include "columnvisibilitymodel.h"
#include "visiblecolumnview.h"
#include "visiblecolumnmodel.h"
#include "visiblecolumndelegate.h"

#include "databaseresourcemodel.h"
#include "databaseresourceview.h"

#include "overwritedialog.h"
#include "balloonmessagedialog.h"

#include "vsizedlayout.h"
#include "hsizedlayout.h"

#include "logger.hpp"

#include <QBoxLayout>
#include <QTabWidget>
#include <QHeaderView>
#include <QListView>
#include <QAbstractTableModel>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QSqlError>

#include <set>

DataBaseEditorWidget::DataBaseEditorWidget(DataBaseTableSchemaModel* schemaModel, DataBaseTableDataModel* dataModel, ColumnVisibilityModel* visibilityModel, VisibleColumnModel* columnModel, DataBaseResourceModel* resourceModel, QWidget* parent)
:
    BalloonWidget(parent)
{
    setTipOrientation(TipOrientation::Left);
    setWindowModality(Qt::NonModal);
    setSizePolicy({ QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding });
    setMaximumSize(600, 600);

    mSchemaView = std::make_unique<DataBaseTableSchemaView>(schemaModel, this);
    mSchemaView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QImage acceptImage(QStringLiteral(":/icons/accept.png"));
    acceptImage.invertPixels(QImage::InvertRgb);

    QImage cancelImage(QStringLiteral(":/icons/cancel.png"));
    cancelImage.invertPixels(QImage::InvertRgb);

    //mGlobalVerticalLayout = new QVBoxLayout(this);
    mGlobalVerticalLayout = new VSizedLayout({1278, 718}, this);
    mGlobalVerticalLayout->setObjectName(QStringLiteral("globalVerticalLayout"));
    mGlobalVerticalLayout->setSizeConstraint(QLayout::SetFixedSize);
    mGlobalVerticalLayout->setContentsMargins(tipHeight() + 5, 5, 5, 5);    
    
    QTabWidget* mTabWidget = new QTabWidget(this);
    mTabWidget->setObjectName(QStringLiteral("tabWidget"));
    mTabWidget->setTabPosition(QTabWidget::North);
    mTabWidget->setTabShape(QTabWidget::Rounded);
    mTabWidget->setTabBarAutoHide(false);
    
    QWidget* databaseTab = new QWidget(mTabWidget);
    databaseTab->setObjectName(QStringLiteral("dataBaseTab"));    
    QVBoxLayout* databaseTabVerticalLayout = new QVBoxLayout(databaseTab);
    databaseTabVerticalLayout->setObjectName(QStringLiteral("dataBaseTabLayout"));
        
    mSchemaEditingWidget = new QWidget(databaseTab);
    mSchemaEditingWidget->setSizePolicy({ QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding });

    mFinishEditingButton = new QPushButton(mSchemaEditingWidget);
    mFinishEditingButton->setIcon(QPixmap::fromImage(acceptImage));
    mFinishEditingButton->setObjectName(QStringLiteral("FinishEditingField"));
    mFinishEditingButton->setToolTip("Finish the edition of the table schema");
    mFinishEditingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mFinishEditingButton->setMaximumSize({ 30, 30 });

    mCancelEditingButton = new QPushButton(mSchemaEditingWidget);
    mCancelEditingButton->setIcon(QPixmap::fromImage(cancelImage));
    mCancelEditingButton->setObjectName(QStringLiteral("mCancelEditingButton"));
    mCancelEditingButton->setToolTip("Cancel the edition of the table schema");
    mCancelEditingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mCancelEditingButton->setMaximumSize({ 30, 30 });

    mAddFieldButton = new QPushButton("+", mSchemaEditingWidget);
    mAddFieldButton->setObjectName(QStringLiteral("addField"));
    mAddFieldButton->setToolTip("Add new field to the table");
    mAddFieldButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mAddFieldButton->setMaximumSize({ 30, 30 });

    mDeleteFieldButton = new QPushButton("-", mSchemaEditingWidget);
    mDeleteFieldButton->setObjectName(QStringLiteral("deleteField"));
    mDeleteFieldButton->setObjectName("Delete selected table field");
    mDeleteFieldButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mDeleteFieldButton->setMaximumSize({ 30, 30 });

    QVBoxLayout* schemaEditingButtonLayout = new QVBoxLayout();
    schemaEditingButtonLayout->setObjectName(QStringLiteral("schemaEditingButtonLayout"));
    //schemaEditingButtonLayout->setContentsMargins(tipHeight(), 0, 0, 0);    
    schemaEditingButtonLayout->setSpacing(0);
    schemaEditingButtonLayout->addWidget(mFinishEditingButton);
    schemaEditingButtonLayout->addWidget(mCancelEditingButton);
    schemaEditingButtonLayout->addStretch(1);

    QHBoxLayout* schemaFieldButtonLayout = new QHBoxLayout();
    schemaFieldButtonLayout->setObjectName(QStringLiteral("schemaEditingButtonLayout2"));
    schemaFieldButtonLayout->addStretch(1);
    schemaFieldButtonLayout->addWidget(mDeleteFieldButton);
    schemaFieldButtonLayout->addWidget(mAddFieldButton);    
    schemaFieldButtonLayout->addStretch(1);
    
    QVBoxLayout* schemaViewLayout = new QVBoxLayout();
    schemaViewLayout->setObjectName(QStringLiteral("schemaViewLayout"));
    schemaViewLayout->setContentsMargins(0, 0, 0, 0);
    schemaViewLayout->addWidget(mSchemaView.get());
    schemaViewLayout->addLayout(schemaFieldButtonLayout);
    schemaViewLayout->addStretch(1);

    QHBoxLayout* mSchemaEditingLayout = new QHBoxLayout();
    //QHBoxLayout* mSchemaEditingLayout = new DataBaseEditorLayout(this, { 600, 720 });
    mSchemaEditingLayout->setObjectName(QStringLiteral("schemaEditingLayout"));
    mSchemaEditingLayout->setSizeConstraint(QLayout::SetFixedSize);
    mSchemaEditingLayout->setContentsMargins(0, 0, 0, 0);        
    mSchemaEditingLayout->addLayout(schemaViewLayout);
    mSchemaEditingLayout->addLayout(schemaEditingButtonLayout);
    mSchemaEditingLayout->addStretch(1);
    
    mSchemaEditingWidget->setLayout(mSchemaEditingLayout);
    
    databaseTabVerticalLayout->addWidget(mSchemaEditingWidget);

    mDataEditingWidget = new QWidget(databaseTab);
    mDataEditingWidget->hide();    
    
    mEditSchemaButton = new QPushButton("E", mDataEditingWidget);
    mEditSchemaButton->setObjectName(QStringLiteral("editSchema"));
    mEditSchemaButton->setToolTip("Edit table schema");
    mEditSchemaButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mEditSchemaButton->setMaximumSize({ 30, 30 });   

    mFinishTableEditingButton = new QPushButton(mDataEditingWidget);
    mFinishTableEditingButton->setIcon(QPixmap::fromImage(acceptImage));
    mFinishTableEditingButton->setObjectName(QStringLiteral("FinishTableEditingField"));
    mFinishTableEditingButton->setToolTip("Close the database widget");
    mFinishTableEditingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mFinishTableEditingButton->setMaximumSize({ 30, 30 });
    
    mCancelTableEditingButton = new QPushButton(mDataEditingWidget);
    mCancelTableEditingButton->setIcon(QPixmap::fromImage(cancelImage));
    mCancelTableEditingButton->setObjectName(QStringLiteral("mCancelTableEditingButton"));
    mCancelTableEditingButton->setToolTip("Cancel the edition of the table");
    mCancelTableEditingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mCancelTableEditingButton->setMaximumSize({30, 30});
    mCancelTableEditingButton->hide();

    mAddRowButton = new QPushButton("+", mDataEditingWidget);
    mAddRowButton->setObjectName(QStringLiteral("addRow"));
    mAddRowButton->setToolTip("Add new row to the table");
    mAddRowButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mAddRowButton->setMaximumSize({ 30, 30 });

    mDelRowButton = new QPushButton("-", mDataEditingWidget);
    mDelRowButton->setObjectName(QStringLiteral("deleteRow"));
    mDelRowButton->setToolTip("Delete the selected table row");
    mDelRowButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mDelRowButton->setMaximumSize({ 30, 30 });

    mCopyTableToClipboardButton = new QPushButton("C", mDataEditingWidget);
    mCopyTableToClipboardButton->setObjectName(QStringLiteral("CopyTableToClipboardButton"));
    mCopyTableToClipboardButton->setToolTip("Copy the table contents to the clipboard");
    mCopyTableToClipboardButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mCopyTableToClipboardButton->setMaximumSize({ 30, 30 });
    
    mOpenVisibilityWidgetButton = new QPushButton(mDataEditingWidget);
    mOpenVisibilityWidgetButton->setObjectName(QStringLiteral("mOpenVisibilityWidgetButton"));
    mOpenVisibilityWidgetButton->setToolTip("Change the visibility of the table fields");
    mOpenVisibilityWidgetButton->setIcon(QIcon(QStringLiteral(":/icons/visible.png")));
    mOpenVisibilityWidgetButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mOpenVisibilityWidgetButton->setMaximumSize({ 30, 30 });
    mOpenVisibilityWidgetButton->setCheckable(true);
    
    QVBoxLayout* dataEditingButtonLayout = new QVBoxLayout();
    dataEditingButtonLayout->setObjectName(QStringLiteral("dataEditingButtonLayout"));
    //dataEditingButtonLayout->setContentsMargins(tipHeight(), 0, 0, 0);    
    dataEditingButtonLayout->setSpacing(0);
    dataEditingButtonLayout->addWidget(mEditSchemaButton);
    dataEditingButtonLayout->addSpacing(10);
    dataEditingButtonLayout->addWidget(mFinishTableEditingButton);
    //dataEditingButtonLayout->addWidget(mCancelTableEditingButton);
    dataEditingButtonLayout->addSpacing(10);
    dataEditingButtonLayout->addWidget(mAddRowButton);
    dataEditingButtonLayout->addWidget(mDelRowButton);
    dataEditingButtonLayout->addWidget(mCopyTableToClipboardButton);
    dataEditingButtonLayout->addWidget(mOpenVisibilityWidgetButton);
    dataEditingButtonLayout->addStretch(1);

    QVBoxLayout* dataViewLayout = new QVBoxLayout();
    dataViewLayout->setObjectName(QStringLiteral("dataViewLayout"));
    dataViewLayout->setContentsMargins(0, 0, 0, 0);
    
    QHBoxLayout* dbFilterLayout = new QHBoxLayout();
    
    mFilterLineEdit = new QLineEdit(mDataEditingWidget);    
        
    dbFilterLayout->addWidget(mFilterLineEdit);

    QPushButton* applyFilterButton = new QPushButton("Filter", databaseTab);
    applyFilterButton->setObjectName(QStringLiteral("applyFilterButton"));
    applyFilterButton->setToolTip("Apply the filter to the table view");

    dbFilterLayout->addWidget(applyFilterButton);

    QPushButton* clearFilterButton = new QPushButton("Clear", databaseTab);
    clearFilterButton->setObjectName(QStringLiteral("clearFilterButton"));
    clearFilterButton->setToolTip("Clear the current filter");
    clearFilterButton->setHidden(true);

    dbFilterLayout->addWidget(clearFilterButton);

    dataViewLayout->addLayout(dbFilterLayout);
    
    mDataView = std::make_unique<DataBaseTableDataView>(dataModel, false, mDataEditingWidget);
    mDataView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        
    dataViewLayout->addWidget(mDataView.get());    
    dataViewLayout->addStretch(1);
    
    mColumnVisibilityWidget = std::make_unique<QWidget>(mDataEditingWidget);
    mColumnVisibilityWidget->setHidden(true);
    
    auto visibilityLayout = new QVBoxLayout();
    visibilityLayout->setObjectName(QStringLiteral("visibilityLayout"));
    
    mVisibilityView = std::make_unique<QListView>(mColumnVisibilityWidget.get());
    mVisibilityView->setMaximumWidth(150);
    mVisibilityView->setResizeMode(QListView::Adjust);
    mVisibilityView->setModel(visibilityModel);
    mVisibilityView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    
    visibilityLayout->addWidget(mVisibilityView.get());
    
    mApplyVisibilityButton = new QPushButton("Apply", this);
    mApplyVisibilityButton->setObjectName(QStringLiteral("applyButton"));
    mApplyVisibilityButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    visibilityLayout->addWidget(mApplyVisibilityButton);
    
    mColumnVisibilityWidget->setLayout(visibilityLayout);

    QHBoxLayout* dataEditingLayout = new HSizedLayout({ 1230, 720 });
    dataEditingLayout->setObjectName(QStringLiteral("dataEditingLayout"));
    dataEditingLayout->setContentsMargins(0, 0, 0, 0);
    dataEditingLayout->addLayout(dataViewLayout);
    dataEditingLayout->addWidget(mColumnVisibilityWidget.get());
    dataEditingLayout->addLayout(dataEditingButtonLayout);
    dataEditingLayout->addStretch(1);
    
    mDataEditingWidget->setLayout(dataEditingLayout);
    
    databaseTabVerticalLayout->addWidget(mDataEditingWidget);
    
    mVisibleColumnLayout = new QHBoxLayout();
    mVisibleColumnLayout->setObjectName(QStringLiteral("visibleColumnLayout"));
    mVisibleColumnLayout->setContentsMargins(0, 0, 0, 0);
    
    mVisibleColumns = std::make_unique<VisibleColumnView>(databaseTab);
    mVisibleColumns->setModel(columnModel);
    mVisibleColumns->setItemDelegate(new VisibleColumnDelegate());    
    mVisibleColumns->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mVisibleColumns->horizontalHeader()->hide();
    mVisibleColumns->verticalHeader()->hide();
    mVisibleColumns->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mVisibleColumns->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);    
    mVisibleColumns->setSelectionBehavior(QAbstractItemView::SelectColumns);
    mVisibleColumns->setSelectionMode(QAbstractItemView::MultiSelection);    
    mVisibleColumns->setRowHeight(0, 10);
    mVisibleColumns->setMaximumHeight(14);
    mVisibleColumns->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mVisibleColumns->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    mVisibleColumns->hide();
    
    mVisibleColumnLayout->addWidget(mVisibleColumns.get());
    mVisibleColumnLayout->addStretch(1);
    
    databaseTabVerticalLayout->addLayout(mVisibleColumnLayout);
    databaseTabVerticalLayout->addStretch(1);
        
    mTabWidget->addTab(databaseTab, "Database");

    
    QWidget* resourceTab = new QWidget(mTabWidget);
    resourceTab->setObjectName("Resource Tab");
    QVBoxLayout* resourceTabVerticalLayout = new QVBoxLayout(resourceTab);
    resourceTabVerticalLayout->setObjectName(QStringLiteral("resourceTabLayout"));    
    
    mResourceView = std::make_unique<DataBaseResourceView>(resourceModel, resourceTab);
    mResourceView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    resourceTabVerticalLayout->addWidget(mResourceView.get());    
    
    QHBoxLayout* resourceButtonLayout = new QHBoxLayout();
    resourceButtonLayout->setObjectName(QStringLiteral("resourceButtonLayout"));
    resourceButtonLayout->setContentsMargins(0, 0, 0, 0);
    
    mImportResourceButton = new QPushButton("Import", resourceTab);
    mImportResourceButton->setObjectName(QStringLiteral("importResource"));
    mImportResourceButton->setToolTip("Import new resource to the layer resource directory");
    mImportResourceButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mImportResourceButton->setMaximumHeight(30);

    mDeleteResourceButton = new QPushButton("Delete", resourceTab);
    mDeleteResourceButton->setObjectName(QStringLiteral("deleteResource"));
    mDeleteResourceButton->setToolTip("Delete the selected resource");
    mDeleteResourceButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mDeleteResourceButton->setMaximumHeight(30);

    resourceButtonLayout->addStretch(1);
    resourceButtonLayout->addWidget(mImportResourceButton);
    resourceButtonLayout->addWidget(mDeleteResourceButton);
    resourceButtonLayout->addStretch(1);
    
    resourceTabVerticalLayout->addLayout(resourceButtonLayout);    
        
    mTabWidget->addTab(resourceTab, "Resources");

    
    mGlobalVerticalLayout->addWidget(mTabWidget);
    mGlobalVerticalLayout->addStretch(1);

    setLayout(mGlobalVerticalLayout);     

    
    mErrorDialog = std::make_unique<BalloonMessageDialog>(this);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() /*| Qt::WindowTransparentForInput*/ | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setTipOrientation(BalloonDialog::TipOrientation::Up);    
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed);    

    selectEditingMode();

    connect(schemaModel, &DataBaseTableSchemaModel::dataChanged, this, &DataBaseEditorWidget::updateSizes);
    connect(schemaModel, &DataBaseTableSchemaModel::dataChanged, this, &DataBaseEditorWidget::hideErrorMessage);
    connect(schemaModel, &DataBaseTableSchemaModel::errorMessageGenerated, this, &DataBaseEditorWidget::showErrorInSchemaEditing);     
    connect(dataModel, &DataBaseTableDataModel::dataChanged, this, &DataBaseEditorWidget::updateVisibleColumns);
    connect(mDataView->horizontalHeader(), &QHeaderView::sectionResized, this, &DataBaseEditorWidget::updateVisibleColumns);
    
    connect(mFilterLineEdit, &QLineEdit::editingFinished, this, &DataBaseEditorWidget::filterTable);
    connect(applyFilterButton, &QPushButton::released, this, &DataBaseEditorWidget::filterTable);
    connect(clearFilterButton, &QPushButton::released, this, &DataBaseEditorWidget::clearFilterTable);

    connect(mAddFieldButton, &QPushButton::released, schemaModel, &DataBaseTableSchemaModel::addField);
    connect(mDeleteFieldButton, &QPushButton::released, mSchemaView.get(), &DataBaseTableSchemaView::deleteSelectedField);
    connect(mFinishEditingButton, &QPushButton::released, schemaModel, &DataBaseTableSchemaModel::createTableSchemaStatement);
    connect(mFinishEditingButton, &QPushButton::released, this, &DataBaseEditorWidget::finishSchemaEditing);
    connect(mCancelEditingButton, &QPushButton::released, this, &DataBaseEditorWidget::cancelSchemaEditing);

    connect(mAddRowButton, &QPushButton::released, this, &DataBaseEditorWidget::addRow);
    connect(mDelRowButton, &QPushButton::released, this, &DataBaseEditorWidget::deleteRow);
    connect(mFinishTableEditingButton, &QPushButton::released, this, &DataBaseEditorWidget::finishEditing);
    connect(mCancelTableEditingButton, &QPushButton::released, this, &DataBaseEditorWidget::cancelEditing);
    connect(mEditSchemaButton, &QPushButton::released, this, &DataBaseEditorWidget::editSchema);
    connect(mCopyTableToClipboardButton, &QPushButton::released, this, &DataBaseEditorWidget::copyTableToClipboard);
    
    connect(mOpenVisibilityWidgetButton, &QPushButton::toggled, this, &DataBaseEditorWidget::toggleColumnVisibilityWidget);
    connect(mApplyVisibilityButton, &QPushButton::released, this, &DataBaseEditorWidget::updateColumnVisibility);

    connect(mImportResourceButton, &QPushButton::released, this, &DataBaseEditorWidget::importResource);
    connect(mDeleteResourceButton, &QPushButton::released, this, &DataBaseEditorWidget::deleteResource);
}

DataBaseEditorWidget::~DataBaseEditorWidget()
{

}

// *** Public Methods *** //

QSize DataBaseEditorWidget::sizeHint() const
{
    auto max = maximumSize();
    auto hint = QWidget::sizeHint();

    if (hint.width() > max.width())
        hint.setWidth(max.width());
    if (hint.height() > max.height())
        hint.setHeight(max.height());

    return hint;
}

bool DataBaseEditorWidget::selectId(int id)
{
    return mDataView->selectNewId(id);
}

bool DataBaseEditorWidget::selectRow(int row)
{
    if(row < mDataView->model()->columnCount())
    {        
        mDataView->selectionModel()->setCurrentIndex(mDataView->model()->index(row, 0), QItemSelectionModel::SelectCurrent);
        mDataView->selectionModel()->select(mDataView->model()->index(row, 0), QItemSelectionModel::SelectCurrent);
        //mDataView->selectNewIndex(mDataView->model()->index(row, 0));
        return true;
    }
    
    return false;
}

void DataBaseEditorWidget::setLayer(unsigned int layer)
{
    if(mLayerIndex != layer)
        clearFilterTable();
    
    mLayerIndex = layer;
    selectEditingMode();
}

void DataBaseEditorWidget::setHiddenColumns(const std::vector<int>& hiddenColumns)
{
/*    auto selectionModel = mVisibleColumns->selectionModel();
    auto dataModel = static_cast<VisibleColumnModel*>(mVisibleColumns->model());
    
    std::vector<int> hidden;
    std::vector<int> visible;
    
    for(auto i = 0; i < hiddenColumns.size(); ++i)        
        if(hiddenColumns[i] != 0)
        {
            hidden.push_back(hiddenColumns[i] - 1);
            selectionModel->select(dataModel->index(0, hidden.back()), QItemSelectionModel::Deselect);
        }
    
    dataModel->clearColumns();
    
    for(int i = 0; i < dataModel->columnCount(); ++i)
    {
        auto search = std::find(begin(hidden), end(hidden), i);
        if(search == end(hidden))
        {
            selectionModel->select(dataModel->index(0, i), QItemSelectionModel::Select);
            dataModel->setSecondaryColumn(i);
        }
    } */   
}

void DataBaseEditorWidget::setResourceDirectory(const std::string& layerName)
{
    auto resourceModel = static_cast<DataBaseResourceModel *>(mResourceView->model());    
    mResourceView->setCurrentIndex(resourceModel->index(resourceModel->rootPath() + "/" + QString::fromStdString(layerName)));
}

void DataBaseEditorWidget::clearResourceDirectorySelection()
{
    mResourceView->clearSelection();
}


// *** Public Slots *** //

void DataBaseEditorWidget::hide()
{
    hideErrorMessage();
    BalloonWidget::hide();
}

void DataBaseEditorWidget::filterTable()
{
    auto dataModel = static_cast<DataBaseTableDataModel*>(mDataView->model());
    dataModel->setFilter(mFilterLineEdit->text());
    
    if(!dataModel->lastError().isValid())
    {
        mErrorDialog->clearMessage();        
        hideErrorMessage();
    }
    else
    {        
        auto error = dataModel->lastError().databaseText();
        dataModel->setFilter("");        
        dataModel->select();
        
        mErrorDialog->setMessage(error, Qt::white); 
        mErrorDialog->adjustSize();
        mErrorDialog->show();
        mErrorDialog->move(errorMessagePosition());
    }
}

void DataBaseEditorWidget::clearFilterTable()
{
    auto dataModel = static_cast<DataBaseTableDataModel*>(mDataView->model());
    mFilterLineEdit->clear();
    dataModel->setFilter("");
    
    hideErrorMessage();
}

void DataBaseEditorWidget::updateSizes()
{
    //     mSchemaEditingWidget->updateGeometry();
    //     updateGeometry();
    // 
    //     auto minSchemaEditing = mSchemaEditingWidget->layout()->minimumSize();
    //     auto hintSchemaEditing = mSchemaEditingWidget->layout()->sizeHint();
    //     auto min = layout()->minimumSize();
    //     auto hint = layout()->sizeHint();
    //     
    //     auto size = sizeHint();
    //     
    //     //adjustSize();    
    //     resize(sizeHint());
}

void DataBaseEditorWidget::addRow()
{
    if(!mFilterLineEdit->text().isEmpty())
        clearFilterTable();
    
    auto dataModel = static_cast<DataBaseTableDataModel*>(mDataView->model());
    dataModel->addEndRow();
    
    static_cast<DataBaseTableDataModel*>(mDataView->model())->commitData();    
}

void DataBaseEditorWidget::deleteRow()
{
    auto selectedIndexes = mDataView->selectionModel()->selectedIndexes();

    std::set<int, std::greater<int>> selectedRows;
    for(const auto& index : selectedIndexes)
        selectedRows.insert(index.row());
    
    std::string rows;
    
    for(auto row : selectedRows)
    {
        static_cast<DataBaseTableDataModel*>(mDataView->model())->delRow(row);
        rows += std::to_string(row) + " ";
    }

    static_cast<DataBaseTableDataModel*>(mDataView->model())->commitData();
    
    LOG("Selected Rows: ", rows);     
}


void DataBaseEditorWidget::finishEditing()
{
    auto dataModel = static_cast<DataBaseTableDataModel*>(mDataView->model());
    if(!dataModel->isDirty())
        hide();
    else if (dataModel->commitData())
        emit editingFinished();
}

void DataBaseEditorWidget::cancelEditing()
{
    hide();
}

void DataBaseEditorWidget::editSchema()
{
    mEditingSchemaFromTable = true;
    mTempTableSchema = *static_cast<DataBaseTableSchemaModel*>(mSchemaView->model())->schema();
    mTempColumnVisibility = static_cast<ColumnVisibilityModel*>(mVisibilityView->model())->columnVisibility();
    setEditingMode(EditingMode::Schema);
}


void DataBaseEditorWidget::selectEditingMode()
{
    auto schema = static_cast<DataBaseTableSchemaModel*>(mSchemaView->model())->schema();

    if (schema == nullptr || schema->fields().size() == 0)
        setEditingMode(EditingMode::Schema);
    else
        setEditingMode(EditingMode::Table);
}

void DataBaseEditorWidget::showErrorInSchemaEditing(const QString& message, int row, int column)
{
    mErrorDialog->setMessage(message, Qt::white);
    mBadEditedCellIndices = {row, column};

    mErrorDialog->adjustSize();
    mErrorDialog->show();
    mErrorDialog->move(errorMessagePosition());    
}

void DataBaseEditorWidget::hideErrorMessage()
{
    if(mErrorDialog->isVisible()) mErrorDialog->hide();
}



// *** Private slots *** //

void DataBaseEditorWidget::finishSchemaEditing()
{
    auto dataModel = static_cast<DataBaseTableDataModel*>(mDataView->model());
    dataModel->loadTable();

    static_cast<ColumnVisibilityModel*>(mVisibilityView->model())->updateVisibilityWithSourceLayer();
    updateColumnVisibility();
    
    std::vector<bool> secondaries(mDataView->model()->columnCount(), false);
    static_cast<VisibleColumnModel*>(mVisibleColumns->model())->setColumns(0, secondaries);    
    updateVisibleColumns();
    
    setEditingMode(EditingMode::Table);
    
    // Reseting y position
    move(pos().x(), 0);
    //emit schemaEditingFinished();
}

void DataBaseEditorWidget::cancelSchemaEditing()
{
    auto schemaModel = static_cast<DataBaseTableSchemaModel*>(mSchemaView->model());
    
    if (mEditingSchemaFromTable)
    {
        mEditingSchemaFromTable = false;
                
        schemaModel->copySchemaContents(mTempTableSchema);
        auto visibilityModel = static_cast<ColumnVisibilityModel*>(mVisibilityView->model());
        visibilityModel->setColumnVisibility(mTempColumnVisibility);
        
        setEditingMode(EditingMode::Table);
    }
    else
    {
        schemaModel->clearSchema();
        hide();
    }
}

void DataBaseEditorWidget::updateVisibleColumns()
{
    if(mVisibleColumns->model()->columnCount() != (mDataView->model()->columnCount()))
        static_cast<VisibleColumnModel *>(mVisibleColumns->model())->setColumnCount(mDataView->model()->columnCount());
    
    for(int i = 0; i < mVisibleColumns->model()->columnCount(); ++i)
        mVisibleColumns->setColumnWidth(i, mDataView->columnWidth(i));
}

void DataBaseEditorWidget::importResource()
{
    auto resourceModel = static_cast<DataBaseResourceModel*>(mResourceView->model());
    auto schema = static_cast<DataBaseTableSchemaModel*>(mSchemaView->model())->schema();
    
    QFileDialog importDialog;
    importDialog.setLabelText(QFileDialog::Accept, "Import");
    importDialog.setFileMode(QFileDialog::ExistingFiles);
    
    if (importDialog.exec())
    {
        auto layerResourcePath = resourceModel->rootPath() + "/" + QString::fromStdString(schema->name());
        bool cancelOperation = false;
        //for(const auto& selectedFilePath : importDialog.selectedFiles())
        for(int i = 0; i < importDialog.selectedFiles().size() && !cancelOperation; ++i)
        {
            auto selectedFilePath = importDialog.selectedFiles().at(i);
            QFileInfo fileInfo(selectedFilePath);
            
            auto newFilePath = layerResourcePath + "/" + fileInfo.fileName();            
            QFileInfo newFileInfo(newFilePath);
            
            QDir dir(layerResourcePath);
            
            if(!dir.exists(newFileInfo.fileName()))
                QFile::copy(selectedFilePath, newFilePath);
            else
            {
                auto fileNames = dir.entryList(QDir::Files);
                    
                OverwriteDialog dialog(fileInfo, newFileInfo, fileNames);
                if (dialog.exec())
                {
                    if (dialog.renamed())
                    {
                        newFilePath = layerResourcePath + "/" + dialog.newName();
                        QFile::copy(selectedFilePath, newFilePath);
                    }
                    else if (dialog.overwritten())
                    {
                        QFile::remove(newFileInfo.absoluteFilePath());
                        QFile::copy(selectedFilePath, newFilePath);
                    }
                }
                else
                    cancelOperation = true;
            }
        }
        emit resourceImported();
    }    
}

void DataBaseEditorWidget::deleteResource()
{
    auto resourceModel = static_cast<DataBaseResourceModel*>(mResourceView->model());
    auto schema = static_cast<DataBaseTableSchemaModel*>(mSchemaView->model())->schema();

    if (!resourceModel->isDir(mResourceView->currentIndex()))
    {
        auto filename = resourceModel->fileName(mResourceView->currentIndex());
        
        if(schema->containFieldType(DataBaseField::Type::Resource))
        {
            auto dataModel = static_cast<DataBaseTableDataModel*>(mDataView->model());        
            std::vector<int> resourceFields;
            
            for(auto i = 0; i < schema->fields().size(); ++i)            
                if(schema->field(i).isResource())
                    resourceFields.push_back(i);
            
            auto tableFilter = schema->field(resourceFields[0]).mName + " = '" + filename.toStdString() + "'";
            
            for(auto i = 1; i < resourceFields.size(); ++i)
                tableFilter += + " OR " + schema->field(resourceFields[i]).mName + " = '" + filename.toStdString() + "'";
            
            auto rowCount = dataModel->rowCount();
            
            dataModel->setFilter(tableFilter.c_str());
            dataModel->select();
            
            rowCount = dataModel->rowCount();
            
            for(auto row = 0; row < dataModel->rowCount(); ++row)
                for(const auto col : resourceFields)
                {
                    auto index = dataModel->index(row, col + 1);
                    if(dataModel->data(index, Qt::DisplayRole).toString() == filename)
                        auto valid = dataModel->setData(index, "");
                }
            
            dataModel->submitAll();
            dataModel->deleteResourceFile(filename.toStdString());
            
            dataModel->setFilter("");
            dataModel->select();
            
        }
        
        resourceModel->remove(mResourceView->currentIndex());        
    }

}

void DataBaseEditorWidget::copyTableToClipboard()
{
    mDataView->copyAll();
}

void DataBaseEditorWidget::updateColumnVisibility()
{
    auto model = static_cast<ColumnVisibilityModel *>(mVisibilityView->model());
    model->applyChanges();
    
    std::vector<int> hidden;    
    auto columnVisibility = model->columnVisibility();
    
    for(auto i = 0; i < columnVisibility.size(); ++i)
        if(!columnVisibility[i])
            hidden.push_back(i + 1);
    
    mDataView->setHiddenColumns(hidden);    
    resize(sizeHint());    
}

void DataBaseEditorWidget::toggleColumnVisibilityWidget()
{
    if(mColumnVisibilityWidget->isVisible())
    {
        auto model = static_cast<ColumnVisibilityModel *>(mVisibilityView->model());
        if(model->isDirty()) model->updateVisibilityWithSourceLayer();
        
        mColumnVisibilityWidget->setHidden(true);        
    }
    else
        mColumnVisibilityWidget->setVisible(true);
}


// *** Protected Methods *** //

void DataBaseEditorWidget::showEvent(QShowEvent* event)
{
    BalloonWidget::showEvent(event);
    
    if(!mErrorDialog->isEmpty())
    {
        mErrorDialog->show();
        mErrorDialog->move(errorMessagePosition());
    }
}

void DataBaseEditorWidget::moveEvent(QMoveEvent* event)
{
    BalloonWidget::moveEvent(event);
    
    if(!mErrorDialog->isEmpty())
    {       
        mErrorDialog->show();
        mErrorDialog->move(errorMessagePosition());
    }    
}


// *** Private Methods *** //

void DataBaseEditorWidget::setEditingMode(EditingMode newMode)
{
    mEditingMode = newMode;

    if (mEditingMode == EditingMode::Table)
    {
        mSchemaEditingWidget->hide();
        mDataEditingWidget->show();
        mVisibleColumns->show();
    }
    else
    {
        mDataEditingWidget->hide();
        mVisibleColumns->hide();
        mSchemaEditingWidget->show();
    }
}

QPoint DataBaseEditorWidget::errorMessagePosition()
{
    QPoint dialogPos;
    if(mEditingMode == EditingMode::Table)
        dialogPos = static_cast<QWidget*>(mFilterLineEdit->parent())->mapToGlobal(mFilterLineEdit->pos() + QPoint(0, mFilterLineEdit->height()));
    else
    {
        int height = mSchemaView->rowViewportPosition(mBadEditedCellIndices.first) +  mSchemaView->rowHeight(mBadEditedCellIndices.first);
        int width = mSchemaView->columnViewportPosition(mBadEditedCellIndices.second) + mSchemaView->columnWidth(mBadEditedCellIndices.second) / 2;
        dialogPos = mSchemaView->viewport()->mapToGlobal({width, height}) - QPoint(mErrorDialog->width() / 2, 0);
    }

    return dialogPos;
}

