#ifndef ACTIVELAYERBUTTONDELEGATE_H
#define ACTIVELAYERBUTTONDELEGATE_H

#include <QStyledItemDelegate>

class ActiveLayerButtonDelegate : public QStyledItemDelegate
{
public:
    explicit ActiveLayerButtonDelegate(QObject* parent = Q_NULLPTR);
    virtual ~ActiveLayerButtonDelegate();
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const Q_DECL_OVERRIDE;
//     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
//     void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
};

#endif // ACTIVELAYERBUTTONDELEGATE_H
