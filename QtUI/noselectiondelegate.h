#ifndef NOSELECTIONDELEGATE_H
#define NOSELECTIONDELEGATE_H

#include <QStyledItemDelegate>

class NoSelectionDelegate: public QStyledItemDelegate
{
    Q_OBJECT    
    
public:
    NoSelectionDelegate(QObject* parent = Q_NULLPTR);
    ~NoSelectionDelegate();
    
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const Q_DECL_OVERRIDE;
};

#endif // PALETTELISTDELEGATE_H
