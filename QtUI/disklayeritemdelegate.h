#ifndef DISKLAYERITEMDELEGATE_H
#define DISKLAYERITEMDELEGATE_H

#include <QStyledItemDelegate>

class DiskLayerItemDelegate: public QStyledItemDelegate
{
public:
    DiskLayerItemDelegate(QObject* parent = Q_NULLPTR);
    
//     void setEditorData(QWidget * editor, const QModelIndex & index) const Q_DECL_OVERRIDE;
//     void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const Q_DECL_OVERRIDE;
    //void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const Q_DECL_OVERRIDE;
};

#endif
