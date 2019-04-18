#include "openglwidget.h"
#include "chisel.h"
#include "renderer.h"
#include "balloonmessagedialog.h"
#include "balloonvaluedialog.h"
#include "databaserowdataview.h"

#include "logger.hpp"

#include <QMouseEvent>
#include <QDebug>

OpenGLWidget::OpenGLWidget(QWidget* parent)
:
    QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    
//     mValueDialog = std::make_unique<BalloonMessageDialog>(qobject_cast< QWidget* >(this->parent()));
//     mValueDialog->setTipOrientation(BalloonDialog::TipOrientation::Down);
//     mValueDialog->setWindowFlags(mValueDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
//     mValueDialog->setAttribute(Qt::WA_ShowWithoutActivating);
//     mValueDialog->setBackgroundColor(Qt::darkGray); 
}

OpenGLWidget::~OpenGLWidget()
{

}

// *** Public Methods ***//

void OpenGLWidget::setChisel(Chisel* chisel)
{
    mChisel = chisel;
}

void OpenGLWidget::initValueDialog(DataBaseTableDataModel* model)
{
    mValueDialog = std::make_unique<BalloonValueDialog>(model, qobject_cast< QWidget* >(this->parent()));
    mValueDialog->setTipOrientation(BalloonDialog::TipOrientation::Down);
    // The dialog doesn't render properly with Qt::WindowTransparentForInput. A non editable stated was added in the model to replicate that funcionality
    mValueDialog->setWindowFlags(mValueDialog->windowFlags() | Qt::WindowDoesNotAcceptFocus); // Qt::WindowTransparentForInput |    
    mValueDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mValueDialog->setBackgroundColor(Qt::darkGray);     
}

void OpenGLWidget::init()
{
    mRenderer->init(defaultFramebufferObject());
}

void OpenGLWidget::showReadValueDialog(float value, bool databaseLayer)
{
    if(databaseLayer)
        mValueDialog->showRowValue(static_cast<int>(value));
    else
        mValueDialog->showValue(value, Qt::white);
    mValueDialog->show();
    mValueDialog->move(mapToGlobal(mMousePos - QPoint(mValueDialog->width()/2, mValueDialog->height())));    
}

void OpenGLWidget::showDataBaseRow(int rowIndex)
{
    mValueDialog->show();
    mValueDialog->showRowValue(rowIndex);
    mValueDialog->move(mapToGlobal(mMousePos - QPoint(mValueDialog->width()/2, mValueDialog->height())));        
}
    
void OpenGLWidget::hideReadValueDialog()
{
    if (mValueDialog->isVisible()) mValueDialog->hide();
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{    
    if(event->button() == Qt::LeftButton)
        mRenderer->onMouseButtons(0, 1, 0, event->pos().x(), event->pos().y());
    else if(event->button() == Qt::RightButton)
    {        
        mRenderer->onMouseButtons(1, 1, 0, event->pos().x(), event->pos().y());
        hideReadValueDialog();
    }
    else if(event->button() == Qt::MiddleButton)
    {
        mRenderer->onMouseButtons(2, 1, 0, event->pos().x(), event->pos().y());
        hideReadValueDialog();
    }
    
    update();
    //QWidget::mousePressEvent(event);
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    mRenderer->onMousePosition(event->pos().rx(), event->pos().ry());
//     qDebug() << "Mouse Position: " << event->pos();
//     qDebug() << "QSize: " << size() << ", frameGeometry" << frameGeometry();
    mMousePos = event->pos();
    update();
    //QWidget::mouseMoveEvent(event);
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
        mRenderer->onMouseButtons(0, 0, 0, event->pos().rx(), event->pos().ry());
    else if(event->button() == Qt::RightButton)
        mRenderer->onMouseButtons(1, 0, 0, event->pos().rx(), event->pos().ry());
    else if(event->button() == Qt::MiddleButton)
        mRenderer->onMouseButtons(2, 0, 0, event->pos().rx(), event->pos().ry());
    
    update();
    //QWidget::mouseReleaseEvent(event);
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    if (!mUpdatingMarkToolRadius)
    {
        mRenderer->onMouseWheel(event->angleDelta().rx() / 40, event->angleDelta().ry() / 40);
        hideReadValueDialog();
    }
    else
        emit markToolRadiusChanged(event->angleDelta().ry() / 4);

    update();
//    QWidget::wheelEvent(event);
}


void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
//     if(event->key() == Qt::Key_A)
//         mRenderer->activePaint(true);
    if(mRenderer->isMarkModeActive() || mRenderer->isEraseModeActive())
    {
        if(event->key() == Qt::Key_S)
            mUpdatingMarkToolRadius = true;
        //else if(event->key() == Qt::Key_A)
        //    mRenderer->resetCamera();
        else if(event->key() == Qt::Key_Z)
        mRenderer->sliceModelWithPlane();
        else if(event->key() == Qt::Key_X)
        mRenderer->setSliceMode(!mRenderer->isSliceModeActive());    
            
    //     else if (event->key() == Qt::Key_R)
    //     {
    //         mRenderer->activeRead(true);
    //     }
    //     else if (event->key() == Qt::Key_E)
    //     {
    //         mRenderer->activeErase(true);
    //     }
    //     else if (event->key() == Qt::Key_Z)
    //         mRenderer->activePaint(false);
    //     else if (event->key() == Qt::Key_X)
    //         mRenderer->activeErase(false);

        update();
    }
//     QWidget::keyPressEvent(event);
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
/*    if(event->key() == Qt::Key_A)
        mRenderer->activePaint(false);
    else */
    
    if(mUpdatingMarkToolRadius && event->key() == Qt::Key_S)
        mUpdatingMarkToolRadius = false;
//     else if (event->key() == Qt::Key_R)
//     {
//         mRenderer->activeRead(false);
//         hideReadValueDialog();
//     }
    //else if (event->key() == Qt::Key_E)
    //{
    //    mRenderer->activeErase(false);
    //}
    
    update();
    //QWidget::keyReleaseEvent(event);
}

void OpenGLWidget::initializeGL()
{
    mChisel->setGLWidget(this);    
    mChisel->init();
    mRenderer = mChisel->renderer();
}

void OpenGLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    makeCurrent();

    mRenderer->resize(w, h);
    
    if(mChisel->currentLayer() != nullptr)
    {
        mChisel->updatePaletteControlPoints();        
    }
    //update();
}

void OpenGLWidget::paintGL()
{
     mRenderer->render();
}

void OpenGLWidget::leaveEvent(QEvent* event)
{
    //-LOG("OpenGLWidget Leave Event");

    if(hasMouseTracking())
    {
        setMouseTracking(false);
        mRenderer->removeMarkTool();
        update();
    }
        
    QOpenGLWidget::leaveEvent(event);
}

void OpenGLWidget::enterEvent(QEvent* event)
{
    //-LOG("OpenGLWidget Enter Event");

    if(!mRenderer->isRenderModeActive())
    {
        setMouseTracking(true);
        if(mRenderer->isMarkModeActive() || mRenderer->isEraseModeActive())
            mRenderer->insertMarkTool();
        update();
    }
    
    if(mRenderer->isResized())
        mRenderer->updateSizeDependentResources();
    
    QOpenGLWidget::enterEvent(event);
}
