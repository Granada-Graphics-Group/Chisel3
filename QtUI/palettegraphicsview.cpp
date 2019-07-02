#include "palettegraphicsview.h"
#include "palette.h"

#include <QMouseEvent>
#include <QDebug>

PaletteGraphicsView::PaletteGraphicsView(QWidget* parent)
:
    QGraphicsView(parent)
{
    mScene = std::make_unique<QGraphicsScene>();   
    setScene(mScene.get());
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

PaletteGraphicsView::~PaletteGraphicsView()
{

}

// *** Public Methods *** //

void PaletteGraphicsView::setPalette(Palette* palette)
{
    mPalette = palette;
        
    updatePaletteScene();
}


// *** Public Slots *** //

void PaletteGraphicsView::updatePaletteScene()
{
    if(mPalette != nullptr)
    {
        auto min = mPalette->minValue();
        auto max = mPalette->maxValue();
        auto viewRect = QRectF(QPoint(0, 0), QPoint(viewport()->width(), viewport()->height()));
        qDebug() << "PaletteGraphicsView::Update Palette: viewport Size: " << viewport()->geometry();
        mScene->clear();

        const auto& ctrlPoints = mPalette->controlPoints();
        
        if(!mPalette->isInterpolating())
        {
            auto colorHeight = viewRect.height() / ctrlPoints.size();
            
            if(colorHeight >= 1)
            {
                for(auto i = 0; i < ctrlPoints.size(); ++i)
                {
                    auto color = mPalette->color(mPalette->controlPoint(i).first);
                    mScene->addRect(0, i * colorHeight, viewRect.width(), colorHeight, { Qt::NoPen }, QBrush(QColor::fromRgbF(color.r, color.g, color.b, color.a)));                
                }
            }
            else
                paintGradient(min, max, viewRect);
        }
        else
        {
            if(mPalette->type() == Palette::Type::Discrete)
            {
                auto count = std::floor(ctrlPoints.rbegin()->first - ctrlPoints.begin()->first) + 1;
                auto colorHeight = viewRect.height() / count;
                
                if(colorHeight >= 1)
                {
                    for (auto i = 0; i < count; ++i)
                    {
                        auto color = mPalette->color(ctrlPoints.begin()->first + i);
                        mScene->addRect(0, i * colorHeight, viewRect.width(), colorHeight, { Qt::NoPen }, QBrush(QColor::fromRgbF(color.r, color.g, color.b, color.a)));
                    }
                }
                else
                    paintGradient(min, max, viewRect);
            }
            else
                paintGradient(min, max, viewRect);
        }
        
        update();
    }
}


// *** Private Slots *** //

void PaletteGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
        emit clickedValue(event->localPos().y()/(viewport()->height()));
        
    QGraphicsView::mousePressEvent(event);
}

void PaletteGraphicsView::moveEvent(QMoveEvent* event)
{
    QWidget::moveEvent(event);
    emit moved();
}


// *** Private Methods *** //

void PaletteGraphicsView::paintGradient(double min, double max, QRectF viewRect)
{
    QLinearGradient gradient(viewRect.topLeft(), viewRect.bottomRight());

    gradient.setSpread(QGradient::RepeatSpread);

    for(const auto& ctrlPoint: mPalette->controlPoints())
    {
        auto color = ctrlPoint.second;
        gradient.setColorAt((ctrlPoint.first - min) / (max - min), QColor::fromRgbF(color.r, color.g, color.b, color.a));
    }

    auto brush = QBrush(gradient);

    mScene->setBackgroundBrush(brush);
}
