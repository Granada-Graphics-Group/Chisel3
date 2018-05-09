#include "databasetableschemaview.h"
#include "databasetableschemadelegate.h"
#include "databasetableschemamodel.h"

#include <QPalette>
#include <QHeaderView>
#include <QScrollBar>

DataBaseTableSchemaView::DataBaseTableSchemaView(DataBaseTableSchemaModel* schemaModel, QWidget* parent)
:
    QTableView(parent)
{       
    setModel(schemaModel);
    
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
    
    setItemDelegateForRow(1, new DataBaseTableSchemaDelegate(schemaModel->fieldTypeString(), this));

    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setVisible(true);
    
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    
    connect(schemaModel, &DataBaseTableSchemaModel::dataChanged, this, &DataBaseTableSchemaView::resizeTableView);    
}

DataBaseTableSchemaView::~DataBaseTableSchemaView()
{

}

// *** Public Methods *** //

QSize DataBaseTableSchemaView::sizeHint() const
{    
    if(horizontalScrollBar()->visibleRegion().isEmpty())
        return QSize(horizontalHeader()->length() + 2 * frameWidth() + verticalHeader()->width(), verticalHeader()->length() + 2 * frameWidth());
    else
        return QSize(horizontalHeader()->length() + 2 * frameWidth() + verticalHeader()->width(), verticalHeader()->length() + 2 * frameWidth() + horizontalScrollBar()->height());

    //return QAbstractScrollArea::sizeHint();
}

QSize DataBaseTableSchemaView::minimumSizeHint() const
{
    return{20, 20};
}


// *** Public Slots *** //

void DataBaseTableSchemaView::deleteSelectedField()
{
    auto index = selectionModel()->currentIndex();
    
    if(index.isValid())
        static_cast<DataBaseTableSchemaModel*>(model())->deleteField(index.column());
}


void DataBaseTableSchemaView::resizeTableView()
{
    //resize(horizontalHeader()->length() + 2 * frameWidth(), verticalHeader()->length() + 2 * frameWidth());    
    //updateGeometry();
}
