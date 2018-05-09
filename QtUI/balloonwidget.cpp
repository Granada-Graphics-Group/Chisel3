#include "balloonwidget.h"

#include <QPainter>

BalloonWidget::BalloonWidget(QWidget* parent)
:
    QWidget(parent)
{
    mBackgroundColor = palette().window().color();
}


// *** Public Methods *** //

QSize BalloonWidget::sizeHint() const
{
    return QSize(200, 60);
}

void BalloonWidget::setTipOrientation(BalloonWidget::TipOrientation orientation)
{
    mTipOrientation = orientation;
    updateShapes();
}

void BalloonWidget::setTipOffset(int offset)
{
    if(mTipOrientation == TipOrientation::Up || mTipOrientation == TipOrientation::Down)
    {
        if (abs(offset) < width())
            mTipOffset = offset;
    }
    else if(abs(offset) < height())
        mTipOffset = offset;
    updateShapes();
}

void BalloonWidget::setTipPosition(const QPoint& pos)
{
    mTipPosition = pos;
    
    updateShapes();
}

void BalloonWidget::resetTipPosition()
{
    switch(mTipOrientation)
    {
        case TipOrientation::Up:
        {
            mTipPosition = {width()/2, 0};
        }
        break;
        
        case TipOrientation::Left:
        {
            mTipPosition = {0, height()/2};
        }
        break;
        
        case TipOrientation::Right:
        {
            mTipPosition = {width(), height()/2};
        }
        break;
        
        case TipOrientation::Down:
        {
            mTipPosition = {width()/2, height()};
        }
    }
    
    updateShapes();
}



// *** Protected Methods *** //

void BalloonWidget::paintEvent(QPaintEvent* event)
{    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(mBackgroundColor);    
    painter.drawPolygon(mTipShape);
    painter.drawRect(mQuad);
    
    QWidget::paintEvent(event);
}

void BalloonWidget::resizeEvent(QResizeEvent* event)
{
    emit resized();

    updateShapes();
    QWidget::resizeEvent(event);
}

void BalloonWidget::moveEvent(QMoveEvent* event)
{
    QWidget::moveEvent(event);

    emit moved();
}


// *** Private Methods *** //

void BalloonWidget::updateShapes()
{
    switch(mTipOrientation)
    {
        case TipOrientation::Up:
        {
            if(!mFixedTipPosition)
            {
                mTipShape.clear();
                mTipShape << QPoint(width()/2 - mTipSize.width()/2 + mTipOffset, mTipSize.height());
                mTipShape << QPoint(width()/2 + mTipSize.width()/2  + mTipOffset, mTipSize.height());
                mTipShape << QPoint(width()/2 + mTipOffset, 0);
                
                mTipPosition = {width()/2, 0};
            }
            
            mQuad.setRect(0, mTipSize.height(), width(), height() - mTipSize.height());
        }
        break;
        
        case TipOrientation::Left:
        {
            if(!mFixedTipPosition)
            {
                mTipShape.clear();
                mTipShape << QPoint(mTipSize.height(), height()/2 - mTipSize.width()/2 + mTipOffset);
                mTipShape << QPoint(0, height()/2 + mTipOffset);
                mTipShape << QPoint(mTipSize.height(), height()/2 + mTipSize.width()/2 + mTipOffset);
                
                mTipPosition = {0, height()/2};
            }
            else
            {
                mTipShape.clear();
                mTipShape << QPoint(mTipSize.height(), mTipPosition.y() - mTipSize.width() / 2);
                mTipShape << QPoint(0, mTipPosition.y());
                mTipShape << QPoint(mTipSize.height(), mTipPosition.y() + mTipSize.width() / 2);
            }
            
            mQuad.setRect(mTipSize.height(), 0, width() - tipSize().height(), height());
        }
        break;
        
        case TipOrientation::Right:
        {
            if(!mFixedTipPosition)
            {
                mTipShape.clear();
                mTipShape << QPoint(width() - mTipSize.height(), height()/2 - mTipSize.width()/2 + mTipOffset);
                mTipShape << QPoint(width(), height()/2 + mTipOffset);
                mTipShape << QPoint(width() - mTipSize.height(), height()/2 + mTipSize.width()/2 + mTipOffset);
                
                mTipPosition = {width(), height()/2};
            }
            
            mQuad.setRect(0, 0, width() - tipSize().height(), height());
        }
        break;
        
        case TipOrientation::Down:
        {
            if(!mFixedTipPosition)
            {
                mTipShape.clear();
                mTipShape << QPoint(width()/2 - mTipSize.width()/2, height() - mTipSize.height() + mTipOffset);
                mTipShape << QPoint(width()/2 + mTipSize.width()/2, height() - mTipSize.height() + mTipOffset);
                mTipShape << QPoint(width()/2, height() + mTipOffset);
                
                mTipPosition = {width()/2, height()};
            }
            
            mQuad.setRect(0, 0, width(), height() - mTipSize.height());            
        }
    }
    
    QRegion tipRegion(mTipShape);
    QRegion quadRegion(mQuad);
    setMask(tipRegion.united(quadRegion));
}
