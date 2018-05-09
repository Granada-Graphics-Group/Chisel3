#include "activelayertreeview.h"
#include "activelayerdelegate.h"
#include "activelayerbuttondelegate.h"

#include <QMenu>
#include <QDropEvent>
#include <QHeaderView>


ActiveLayerTreeView::ActiveLayerTreeView(QWidget* parent)
:
    QTreeView(parent)
{
    setHeaderHidden(true);
    header()->setCascadingSectionResizes( false );
    
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
        
    auto buttonDelegate = new ActiveLayerButtonDelegate();

    setItemDelegateForColumn(0, new ActiveLayerDelegate());
    setItemDelegateForColumn(1, buttonDelegate);
    setItemDelegateForColumn(2, buttonDelegate);
    setItemDelegateForColumn(3, buttonDelegate);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &ActiveLayerTreeView::customContextMenuRequested, this, &ActiveLayerTreeView::showContextMenu);
}

ActiveLayerTreeView::~ActiveLayerTreeView()
{
}


// *** Public Methods *** //

void ActiveLayerTreeView::setupSectionSize()
{
    header()->setStretchLastSection(false);
	header()->setMinimumSectionSize(32);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->resizeSection(1, 32);
    header()->resizeSection(2, 32);
    header()->resizeSection(3, 32);
}

void ActiveLayerTreeView::dropEvent(QDropEvent* event)
{
    //QModelIndex droppedIndex = indexAt(event->pos());
    //if (!droppedIndex.isValid())
    //    return;

    QAbstractItemView::dropEvent(event);

    //DropIndicatorPosition dp = dropIndicatorPosition();
    //auto selectedIndex = selectionModel()->selectedIndexes().front();
    //QModelIndex selectionIndex;
    //if (dp == QAbstractItemView::BelowItem)
    //{ 
    //        selectionIndex = droppedIndex.sibling(droppedIndex.row() + 1, droppedIndex.column());
    //}
    //else if (dp == QAbstractItemView::AboveItem)
    //{
    //        selectionIndex = droppedIndex.sibling(droppedIndex.row() - 1, droppedIndex.column());
    //}
    // //auto selectedIndex = selectionModel()->selectedIndexes().front();

    // selectionModel()->select(selectionIndex, QItemSelectionModel::Select);
    // selectionModel()->setCurrentIndex(droppedIndex, QItemSelectionModel::Current);
}

// *** Private slots ***//

void ActiveLayerTreeView::showContextMenu(const QPoint& pos)
{
    if(indexAt(pos).isValid())
        mContextMenu->exec(mapToGlobal(pos));
}
