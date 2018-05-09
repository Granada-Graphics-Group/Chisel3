#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QPoint>
#include <memory>

class Renderer;
class Chisel;
class BalloonMessageDialog;
class BalloonValueDialog;
class DataBaseTableDataModel;

class OpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
    
public:
    OpenGLWidget(QWidget* parent = 0);
    ~OpenGLWidget();
    
    Renderer* renderer(){ return mRenderer; }
    
    void setChisel(Chisel* chisel);
    void initValueDialog(DataBaseTableDataModel* model);
    void init();
    
    void showReadValueDialog(float value, bool databaseLayer);
    void showDataBaseRow(int rowIndex);
    void hideReadValueDialog();
    
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE; 
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE;    

signals:
    void markToolRadiusChanged(int increment);
    
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void leaveEvent(QEvent * event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent * event) Q_DECL_OVERRIDE;

private:
    Chisel* mChisel;
    Renderer* mRenderer;
    std::unique_ptr<BalloonValueDialog> mValueDialog;
    
    QPoint mMousePos;
    
    bool mUpdatingMarkToolRadius = false;
};

#endif // OPENGLWIDGET_H
