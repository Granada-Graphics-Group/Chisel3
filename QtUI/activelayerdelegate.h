#ifndef ACTIVELAYERDELEGATE_H
#define ACTIVELAYERDELEGATE_H

#include <QStyledItemDelegate>

class ActiveLayerDelegate : public QStyledItemDelegate
{
public:
    explicit ActiveLayerDelegate(QObject* parent = Q_NULLPTR);
    virtual ~ActiveLayerDelegate();
    
    //void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    //QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const Q_DECL_OVERRIDE;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const Q_DECL_OVERRIDE;
};


#endif


