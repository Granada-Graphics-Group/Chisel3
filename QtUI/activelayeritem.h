#ifndef ACTIVELAYERITEM_H
#define ACTIVELAYERITEM_H

#include <QVariant>
#include <QList>

class ActiveLayerItem
{
public:    
    explicit ActiveLayerItem(const QList<QVariant> &data, ActiveLayerItem *parentItem = 0);
    ~ActiveLayerItem();
    
    const QList<ActiveLayerItem* >& childs() const;
    ActiveLayerItem *child(int row) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    ActiveLayerItem *parentItem();
    
    void setData(const QList<QVariant> &data) { mItemData = data; }
    void setData(int column, const QVariant& data);

    void prependChild(ActiveLayerItem *child);
    void appendChild(ActiveLayerItem *child);
    void insertChild(ActiveLayerItem *child, int row);
    void updateChild(int prevRow, int newRow, const QList<QVariant> &data);
    void removeChild(int row);
    
private:
    QList<ActiveLayerItem*> mChildItems;
    QList<QVariant> mItemData;
    ActiveLayerItem *mParentItem;    
};

#endif // ACTIVELAYERITEM_H
