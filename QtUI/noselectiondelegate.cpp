#include "noselectiondelegate.h"

#include <QPainter>
#include <QApplication>

NoSelectionDelegate::NoSelectionDelegate(QObject* parent)
:
    QStyledItemDelegate(parent)
{

}

NoSelectionDelegate::~NoSelectionDelegate()
{

}


// *** Public Methods *** //

void NoSelectionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{    
    QStyleOptionViewItem noSelectionOption(option);
    noSelectionOption.state &= ~QStyle::State_Selected;
    QStyledItemDelegate::paint(painter, noSelectionOption, index);
}