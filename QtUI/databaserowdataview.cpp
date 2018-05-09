#include "databaserowdataview.h"
#include "databasetabledatamodel.h"
#include "databasetable.h"

#include <QDesktopServices>
#include <QUrl>


// *** Public methods *** //

void DataBaseRowDataView::mouseDoubleClickEvent(QMouseEvent* event)
{
    auto dataModel = static_cast<DataBaseTableDataModel*>(model());
    auto index = selectionModel()->currentIndex();
    
    if(dataModel->schema()->field(index.column() - 1).isResource())
    {        
        auto resource = dataModel->data(index, Qt::DisplayRole).toString();        
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(dataModel->resourceDir()) + "/" + resource));
    }
}

