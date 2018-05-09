#ifndef OPACITYEDITORWIDGET_H
#define OPACITYEDITORWIDGET_H

#include "balloonwidget.h"

#include <memory>

class BalloonMessageDialog;
class QSlider;
class QLineEdit;

class OpacityEditorWidget : public BalloonWidget
{
    Q_OBJECT
    
public:
    OpacityEditorWidget(unsigned int layerIndex, float opacity, QWidget *parent = Q_NULLPTR);
    ~OpacityEditorWidget();
    
    unsigned int layerIndex() const { return mLayerIndex; }
    
    void setLayer(unsigned int index, float opacity);
    void setOpacity(float opacity);
    
public slots:
    void updateOpacityValueFromEditor();
    void updateOpacityValueFromSlider();
    
signals:
    void opacityUpdated(unsigned int layerIndex, float opacity);
    
private:
    std::unique_ptr<QSlider> mSlider;
    std::unique_ptr<QLineEdit> mEditor;    
    std::unique_ptr<BalloonMessageDialog> mErrorDialog; 
    
    unsigned int mLayerIndex;
};

#endif // OPACITYEDITORWIDGET_H
