#include "databasetabledataview.h"
#include "databasetabledatamodel.h"
#include "databasetabledatadelegate.h"
#include "databasetable.h"
#include "logger.hpp"

#include <QHeaderView>
#include <QScrollBar>
#include <QFontMetrics>
#include <QSqlRecord>
#include <QSqlError>
#include <algorithm>

DataBaseTableDataView::DataBaseTableDataView(DataBaseTableDataModel* dataModel, bool resizeToContents, QWidget* parent)
:
    CustomTableView(parent),
    mResizeToContents(resizeToContents)
{   
    setSelectionMode(SingleSelection);
    
    if(!resizeToContents)
        horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    horizontalHeader()->setVisible(true);
    verticalHeader()->setVisible(false);
    
    setModel(dataModel);
    
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    
    connect(dataModel, &DataBaseTableDataModel::modelReset, this, &DataBaseTableDataView::updateColumnVisibility);
    connect(dataModel, &DataBaseTableDataModel::headerDataChanged, this, &DataBaseTableDataView::updateDelegates);
    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &DataBaseTableDataView::selectNewIndex);
    
    mDataDelegate = std::make_unique<DataBaseTableDataDelegate>(this);
    connect(mDataDelegate.get(), &DataBaseTableDataDelegate::closeEditor, dataModel, &DataBaseTableDataModel::commitData);
}

DataBaseTableDataView::~DataBaseTableDataView()
{

}

// *** Public Methods *** //

QSize DataBaseTableDataView::sizeHint() const
{   
    auto length = horizontalHeader()->length();
    auto width = verticalHeader()->width();
    auto fWidth = 2 * frameWidth();
    
    //LOG("Size Hint: ", QTableView::sizeHint().width(), ", ", QTableView::sizeHint().height());
    auto columnsWidth = 0;
    for(int i = 0; i < model()->columnCount(); ++i)
        columnsWidth += columnWidth(i);
    
    //LOG("Size Hint Columns: ", columnsWidth, " returning: ", horizontalHeader()->length() + 2 * frameWidth());
    
    auto newSize = QSize(horizontalHeader()->length() + 2 * frameWidth(), verticalHeader()->length() + 2 * frameWidth() + horizontalHeader()->height());

    if(!horizontalScrollBar()->visibleRegion().isEmpty()) newSize.setHeight(newSize.height() + horizontalScrollBar()->height());
    if (!verticalScrollBar()->visibleRegion().isEmpty()) newSize.setWidth(newSize.width() + verticalScrollBar()->width());

    return newSize;
    //    return QSize(horizontalHeader()->length() + 2 * frameWidth(), verticalHeader()->length() + 2 * frameWidth() + horizontalHeader()->height());
    //else
    //    return QSize(horizontalHeader()->length() + 2 * frameWidth(), verticalHeader()->length() + 2 * frameWidth() + horizontalHeader()->height() + horizontalScrollBar()->height());

//     width = 0;
//     for(int i = 0; i < model()->columnCount(); ++i)
//         width += columnWidth(i);
//     
//     return QSize(width, verticalHeader()->length() + 2 * frameWidth() + horizontalHeader()->height());
}

QSize DataBaseTableDataView::minimumSizeHint() const
{
    return {0, 0};
}

void DataBaseTableDataView::setHiddenColumns(const std::vector<int>& columns)
{
    std::vector<int> difference;    
    std::set_symmetric_difference(begin(mHiddenColumns), end(mHiddenColumns), begin(columns), end(columns), std::back_inserter(difference));    
    for(auto column: difference) showColumn(column);
    
    mHiddenColumns = columns;
    updateColumnVisibility();
}

void DataBaseTableDataView::showHiddenColumns()
{
    for(auto column : mHiddenColumns) showColumn(column);
    mHiddenColumns.clear();
}


// *** Public Slots *** //

bool DataBaseTableDataView::selectNewId(int id)
{
    auto tableModel = static_cast<QSqlRelationalTableModel*>(model());
    auto queryModel = new QSqlQueryModel;
    auto str = QString("SELECT Id FROM " + tableModel->tableName() + " WHERE Id =" + QString::number(id)).toStdString();
    queryModel->setQuery("SELECT Id FROM " + tableModel->tableName() + " WHERE Id =" + QString::number(id));
    
    if(!queryModel->lastError().isValid())
    {
        auto row = queryModel->record(0).value("Id").toInt();
        selectionModel()->select(tableModel->index(row, 0), QItemSelectionModel::Select);
        
        return true;
    }
    
    return false;
}

void DataBaseTableDataView::selectNewIndex(const QModelIndex& index)
{
    auto dbModel = static_cast<DataBaseTableDataModel*>(model());
    emit newIdSelected(dbModel->id(index.row()));
}

void DataBaseTableDataView::updateColumnVisibility()
{
    for(auto column : mHiddenColumns) hideColumn(column);
}


void DataBaseTableDataView::updateDelegates()
{
    auto schema = static_cast<DataBaseTableDataModel*>(model())->schema();
        
    if(schema != nullptr)
    {        
        setItemDelegateForColumn(0, mDataDelegate.get());
        
        if(mResizeToContents)
            horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        
        for(int i = 0; i < schema->fields().size(); ++i)
        {
            setItemDelegateForColumn(i + 1, mDataDelegate.get());

            if(mResizeToContents)
            {

                if(schema->field(i).mType == DataBaseField::Type::Date)
                {
                    horizontalHeader()->setSectionResizeMode(i + 1, QHeaderView::Fixed);
                    QFontMetrics metric(font());
                    auto width = metric.width("dd/MM/yyyy");
                    horizontalHeader()->resizeSection(i + 1, width + 2);
                }
                else
                {
                    horizontalHeader()->setSectionResizeMode(i + 1, QHeaderView::ResizeToContents);
                    horizontalHeader()->setMaximumSectionSize(130);
                }
            }
        }
    }
}


// *** Protected slots *** //

void DataBaseTableDataView::leaveEvent(QEvent* event)
{
    static_cast<DataBaseTableDataModel*>(model())->commitData();
    CustomTableView::leaveEvent(event);
}



// void DataBaseTableDataView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
// {
// 
// }
// 
// QModelIndex DataBaseTableDataView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
// {
// 
// }
