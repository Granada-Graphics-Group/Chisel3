#include "palettelistdelegate.h"
#include "palettelistmodel.h"
#include "palettelistview.h"
#include "palette.h"

#include <QPainter>
#include <QApplication>

PaletteListDelegate::PaletteListDelegate(QObject* parent)
:
    QStyledItemDelegate(parent)
{

}

PaletteListDelegate::~PaletteListDelegate()
{

}


// *** Public Methods *** //

void PaletteListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{    
    Palette* palette = qvariant_cast<Palette*>(index.data(Qt::DisplayRole));
    bool discreteLayerType = qvariant_cast<bool>(index.data(Qt::UserRole));

    QRect rect = option.rect;
    QRect stateRect;
    painter->save();

    // This call will take care to draw, dashed line while selecting
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    const auto& ctrlPoints = palette->controlPoints();    
    auto interpolating = palette->isInterpolating();
    auto discreteCount = interpolating ? std::floor(ctrlPoints.rbegin()->first - ctrlPoints.begin()->first) + 1 : ctrlPoints.size();
    auto colorWidth = rect.width() / discreteCount;
    auto iColorWidth = ceil(colorWidth);    
    auto iColorHeight = ceil(rect.height() / 4);
    
    if(interpolating)
    {    
        if(discreteLayerType && colorWidth >= 1)
        {
            for (auto i = 0; i < discreteCount; ++i)
            {
                auto color = palette->color(ctrlPoints.begin()->first + i);
                painter->setBrush(QBrush(QColor::fromRgbF(color.r, color.g, color.b, color.a)));
                painter->setPen(Qt::NoPen);           
                painter->drawRect(rect.left() + i * iColorWidth, rect.bottomLeft().y() - iColorHeight, iColorWidth,  iColorHeight);            
            }
        }
        else
            paintGradient(painter, rect, palette, iColorHeight); 
    }
    else
    {
        if(colorWidth >= 1)
        {
            for (auto i = 0; i < ctrlPoints.size(); ++i)
            {
                auto color = palette->color(palette->controlPoint(i).first);
                painter->setBrush(QBrush(QColor::fromRgbF(color.r, color.g, color.b, color.a)));
                painter->setPen(Qt::NoPen);           
                painter->drawRect(rect.left() + i * iColorWidth, rect.bottomLeft().y() - iColorHeight, iColorWidth,  iColorHeight);            
            }
        }
        else
            paintGradient(painter, rect, palette, iColorHeight);
    }
    
    painter->restore();
    
    if(mEditing == true && option.state & QStyle::State_Selected)
        QStyledItemDelegate::paint(painter, option, index);   
    else
    {
        painter->save();
        auto title = QString::fromStdString(palette->name());
        QPen fontPen(Qt::black, 1, Qt::SolidLine);
        painter->setPen(fontPen);
        painter->setBrush(Qt::black);
        painter->drawText(rect.left() + 20, rect.top(), rect.width(), rect.height(), Qt::AlignTop|Qt::AlignLeft, title, &rect);

        painter->restore();
    }
}

QSize PaletteListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(200, 30);
}

QWidget * PaletteListDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    mEditing = true;
    
    QWidget * editor = QStyledItemDelegate::createEditor(parent, option, index);
    
    connect( editor, SIGNAL(destroyed(QObject*)), SLOT(onEditorDestroyed(QObject*)));
    
    return editor;    
}


void PaletteListDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void PaletteListDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

void PaletteListDelegate::onEditorDestroyed(QObject* obj)
{
    mEditing = false;
}

// *** Private Methods *** //

void PaletteListDelegate::paintGradient(QPainter* painter, const QRect& rect, Palette* palette, double colorHeight) const
{
    QLinearGradient gradient(QPointF(rect.left(), 0), QPointF(rect.right(), 1));
    gradient.setSpread(QGradient::RepeatSpread);

    auto min = palette->minValue();
    auto max = palette->maxValue();

    for(const auto& ctrlPoint: palette->controlPoints())
    {
        auto color = ctrlPoint.second;
        gradient.setColorAt((ctrlPoint.first - min) / (max - min), QColor::fromRgbF(color.r, color.g, color.b, color.a));
    }
    painter->setBrush(QBrush(gradient));
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect.left(), rect.bottomLeft().y() - colorHeight, rect.width() + 10,  colorHeight); 
}
