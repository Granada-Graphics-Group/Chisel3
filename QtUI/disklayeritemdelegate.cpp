#include "disklayeritemdelegate.h"

#include <QPainter>

DiskLayerItemDelegate::DiskLayerItemDelegate(QObject* parent)
:
    QStyledItemDelegate(parent)
{
    
}

// *** Public Methods *** //

// void DiskLayerItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
// {
//     auto textEditor = static_cast<QLineEdit*>(editor);
//     auto name = index.data().toString();
// //    name.remove(name.indexOf(".lay"), name.length());
//     textEditor->setText(name);
// }
// 
// void DiskLayerItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
// {
//     auto textEditor = static_cast<QLineEdit*>(editor);    
//     model->setData(index, textEditor->text() + ".lay", Qt::EditRole);
// }

// void DiskLayerItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
// {
// //     QRect rect = option.rect;           
// // 
// //     painter->setRenderHint(QPainter::HighQualityAntialiasing);
// //     
// //     if(index.column() == 1)
// //     {
// //         bool loaded = qvariant_cast<bool>(index.data(Qt::DisplayRole));
// //         
// //         if(loaded) 
// //         {
// //             
// //         }
// //         else
// //             QStyledItemDelegate::paint(painter, option, index);
// //     }
//     
// 
// }

QSize DiskLayerItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return  QSize(5, QStyledItemDelegate::sizeHint(option, index).height());
}

