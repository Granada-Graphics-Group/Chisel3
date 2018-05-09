#include "databaseresourceview.h"
#include "databaseresourcemodel.h"

#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>

DataBaseResourceView::DataBaseResourceView(DataBaseResourceModel* model, QWidget* parent)
:
    QTreeView(parent)
{    
    setHeaderHidden(true);
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    setEditTriggers(NoEditTriggers);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);    
    
    connect(model, &DataBaseResourceModel::rootPathChanged, this, &DataBaseResourceView::updateViewModelData);
    
    setModel(model);
}


// *** Public Slots *** //

void DataBaseResourceView::updateViewModelData()
{
    auto resourceModel = dynamic_cast<DataBaseResourceModel*>(model());    
    setRootIndex(resourceModel->index(resourceModel->rootPath()));

    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    header()->resizeSection(1, 7);       

    hideColumn(2);
    hideColumn(3);    
}

void DataBaseResourceView::mouseDoubleClickEvent(QMouseEvent* event)
{
    auto resourceModel = dynamic_cast<DataBaseResourceModel*>(model());
    if(!resourceModel->isDir(selectionModel()->currentIndex()))
    {
        auto file = resourceModel->filePath(selectionModel()->currentIndex());
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    }
}
