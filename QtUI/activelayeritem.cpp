#include "activelayeritem.h"

ActiveLayerItem::ActiveLayerItem(const QList<QVariant> &data, ActiveLayerItem *parentItem)
:
    mItemData(data),
    mParentItem(parentItem)
{

}

ActiveLayerItem::~ActiveLayerItem()
{
    qDeleteAll(mChildItems);
}

// *** Public Methods *** //

const QList< ActiveLayerItem* >& ActiveLayerItem::childs() const
{
    return mChildItems;
}


ActiveLayerItem* ActiveLayerItem::child(int row) const
{
    return mChildItems.value(row);
}

int ActiveLayerItem::childCount() const
{
    return mChildItems.count();
}

int ActiveLayerItem::columnCount() const
{
    return mItemData.count();
}

QVariant ActiveLayerItem::data(int column) const
{
    return mItemData.value(column);
}

int ActiveLayerItem::row() const
{
    if (mParentItem)
        return mParentItem->mChildItems.indexOf(const_cast<ActiveLayerItem*>(this));

    return 0;
}

ActiveLayerItem* ActiveLayerItem::parentItem()
{
    return mParentItem;
}


void ActiveLayerItem::setData(int column, const QVariant& data)
{
    mItemData[column] = data;
}


void ActiveLayerItem::prependChild(ActiveLayerItem* child)
{
    mChildItems.prepend(child);
}

void ActiveLayerItem::appendChild(ActiveLayerItem* child)
{
    mChildItems.append(child);
}

void ActiveLayerItem::insertChild(ActiveLayerItem* child, int row)
{
    mChildItems.insert(row, child);
}

void ActiveLayerItem::updateChild(int prevRow, int newRow, const QList< QVariant >& data)
{
    auto child = mChildItems.value(prevRow);
    child->setData(data);
    mChildItems.removeAt(prevRow);
    mChildItems.insert(newRow, child);
}

void ActiveLayerItem::removeChild(int row)
{
    mChildItems.removeAt(row);
}

