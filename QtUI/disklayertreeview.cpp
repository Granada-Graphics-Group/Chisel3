#include "disklayertreeview.h"
#include "disklayermodel.h"
#include "disklayeritemdelegate.h"

#include <QHeaderView>

DiskLayerTreeView::DiskLayerTreeView(QWidget* parent)
:
    QTreeView(parent)
{
    setHeaderHidden(true);
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    setSelectionBehavior(SelectRows);
    setSelectionMode(ExtendedSelection);
    setEditTriggers(NoEditTriggers);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);    
}


// *** Public Methods *** //

std::vector<std::string> DiskLayerTreeView::selectedLayerNames()
{
    std::vector<std::string> layerNames;
    
    for(const auto& selected : selectionModel()->selectedRows())
        layerNames.emplace_back(selected.data().toString().toStdString());

    return layerNames;
}



// *** Public Slots *** //

void DiskLayerTreeView::updateViewWithModelData()
{
    auto fileModel = dynamic_cast<DiskLayerModel*>(model());    
    setRootIndex(fileModel->index(fileModel->rootPath()));

    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    header()->resizeSection(1, 7);       

    hideColumn(2);
    hideColumn(3);
    
    
    //setItemDelegateForColumn(1, new DiskLayerItemDelegate(this));
}


// *** Protected Methods *** //

void DiskLayerTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{   
    auto layerNames = selectedLayerNames();
    
    emit LayerSelected(layerNames);
    
    QTreeView::mouseDoubleClickEvent(event);
}



