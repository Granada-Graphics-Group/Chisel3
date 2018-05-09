#ifndef BALLOONWIDGET_H
#define BALLOONWIDGET_H

#include <QWidget>

class BalloonWidget : public QWidget
{
    Q_OBJECT

public:
    enum class TipOrientation 
    {
        Up,
        Down,
        Left,
        Right
    };
    
    BalloonWidget(QWidget *parent = Q_NULLPTR); 
    
    QSize sizeHint() const Q_DECL_OVERRIDE;
    const QSize tipSize() const { return mTipSize; }
    const int tipWidth() const { return mTipSize.width(); }
    const int tipHeight() const { return mTipSize.height(); }
    const int tipOffset() const { return mTipOffset; }
    QPoint tipPosition() const { return mTipPosition; };
    TipOrientation tipOrientation() const { return mTipOrientation; }
    bool isTipPositionFixed() const { return mFixedTipPosition; }
    const QColor backgroundColor() const { return mBackgroundColor; }

    void setTipSize(const QSize& size) { mTipSize = size; updateShapes(); }
    void setTipSize(int width, int height) { mTipSize = {width, height}; updateShapes();}
    void setTipWidth(int width) { mTipSize.setWidth(width); updateShapes();}
    void setTipHeight(int height) { mTipSize.setHeight(height); updateShapes();}
    void setTipOffset(int offset);
    void setTipPosition(const QPoint& pos);
    void resetTipPosition();
    void setTipOrientation(TipOrientation orientation);
    void setFixedTipPosition(bool value) { mFixedTipPosition = value; }
    void setBackgroundColor(const QColor& color) { mBackgroundColor = color; }

    void updateShapes();
signals:
    void resized();
    void moved();

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent* event) Q_DECL_OVERRIDE;

private:

    
    TipOrientation mTipOrientation = TipOrientation::Up;
    QPolygon mTipShape;
    QSize mTipSize {30, 14};
    QPoint mTipPosition;
    int mTipOffset = 0;
    bool mFixedTipPosition = false;
    
    QRect mQuad;
    QColor mBackgroundColor;    
};

#endif // BALLOONWIDGET_H
