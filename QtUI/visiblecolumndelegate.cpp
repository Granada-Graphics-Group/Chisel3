#include "visiblecolumndelegate.h"

VisibleColumnDelegate::VisibleColumnDelegate(QObject* parent)
: 
    QStyledItemDelegate(parent)
{
    
}


// *** Public Methods *** //

void VisibleColumnDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem newOption(option);
    newOption.state &= ~QStyle::State_Selected;
    
    QStyledItemDelegate::paint(painter, newOption, index);
}

