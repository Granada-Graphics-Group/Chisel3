#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include <windows.h>
#endif

#include <QApplication>
#include <QDebug>

#include "glrenderer.h"
#include "mainwindow.h"
#include "openglwidget.h"
#include "logger.hpp"

#include <QDesktopWidget>
#include <QSurfaceFormat>
#include <QTimer>
#include <clocale>

int main(int argc, char** argv)
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);    
    
    QSurfaceFormat glFormat;
    glFormat.setVersion(4, 5);
    glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
    glFormat.setRenderableType(QSurfaceFormat::OpenGL);
    glFormat.setOption(QSurfaceFormat::DebugContext | QSurfaceFormat::DeprecatedFunctions);
    QSurfaceFormat::setDefaultFormat(glFormat);    
    
    QApplication app(argc, argv);
    
    log_inst.set_thread_name("MAIN");    
    LOG("CHISel starting ...");
    
    std::setlocale(LC_NUMERIC, "C");
    
    MainWindow mw;
    
    mw.show();
    mw.settingUp();
    QTimer::singleShot(50, &mw, &MainWindow::fixit);
    mw.move(QApplication::desktop()->screen()->rect().center() - mw.rect().center());
    
    return app.exec();
}
