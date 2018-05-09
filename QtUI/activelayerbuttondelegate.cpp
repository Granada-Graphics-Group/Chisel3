#include "activelayerbuttondelegate.h"

#include <QPainter>

ActiveLayerButtonDelegate::ActiveLayerButtonDelegate(QObject* parent)
:
    QStyledItemDelegate(parent)
{

}

ActiveLayerButtonDelegate::~ActiveLayerButtonDelegate()
{

}

void ActiveLayerButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{    
    QRect rect = option.rect;        
    QPixmap icon;    

    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    
    if(!index.parent().isValid())
    {
        if(index.column() == 1)
        {
            bool registerType = qvariant_cast<bool>(index.data(Qt::DisplayRole));
            if(registerType) icon = QPixmap (":/icons/database.png");
        }
        else if(index.column() == 2)
        {
            bool visible = qvariant_cast<bool>(index.data(Qt::DisplayRole));
            (visible) ? icon = QPixmap (":/icons/visible.png") : icon = QPixmap(":/icons/hidden.png");
        }
        else if(index.column() == 3)
        {
            icon = QPixmap(":/icons/opacity.png");
            rect = rect.marginsRemoved({1, 1, 1, 1});
        }
                
        if(!icon.isNull())
            painter->drawPixmap(rect, icon.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

QSize ActiveLayerButtonDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(32, 32);
}


// QWidget* ActiveLayerButtonDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
// {
//     return nullptr;
// }
// 
// void ActiveLayerButtonDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
// {
// 
// }
