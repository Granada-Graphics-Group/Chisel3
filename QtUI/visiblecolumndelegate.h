#ifndef VISIBLECOLUMNDELEGATE_H
#define VISIBLECOLUMNDELEGATE_H

#include <QStyledItemDelegate>

class VisibleColumnDelegate : public QStyledItemDelegate
{
public:
    VisibleColumnDelegate(QObject* parent = Q_NULLPTR);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
};

#endif
