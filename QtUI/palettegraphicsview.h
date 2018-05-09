#ifndef PALETTEGRAPHICSVIEW_H
#define PALETTEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <memory>

class Palette;
class QMouseEvent;

class PaletteGraphicsView : public QGraphicsView
{
    Q_OBJECT
    
public:
    PaletteGraphicsView(QWidget* parent = 0);
    ~PaletteGraphicsView();

    Palette* palette() const { return mPalette; }
    
    void settingUp(Palette* palette);
    void setPalette(Palette* palette);

public slots:
    void updatePaletteScene();
    
signals:
    void clickedValue(double percent);
    void moved();
    
protected:
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent* event)  Q_DECL_OVERRIDE;
    
private:
    void paintGradient(double min, double max, QRectF viewRect);
    
    std::unique_ptr<QGraphicsScene> mScene;
    Palette* mPalette = nullptr;
};

#endif // PALETTEGRAPHICSVIEW_H
