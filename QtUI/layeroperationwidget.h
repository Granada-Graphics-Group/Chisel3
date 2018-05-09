#ifndef LAYEROPERATIONWIDGET_H
#define LAYEROPERATIONWIDGET_H

#include "balloonwidget.h"
#include "customtextedit.h"

#include <memory>

class QVBoxLayout;
class QPushButton;
class QDialogButtonBox;

class LayerOperationWidget : public BalloonWidget
{
    Q_OBJECT

public:
    LayerOperationWidget(QWidget *parent = Q_NULLPTR);
    CustomTextEdit* richEditor(){ return mStatementEditor.get(); }
    void setEditorText(const std::string& text);
    void setErrorLogText(const std::string& text);
    void setEditorFocus();
    void setMaxAndMinSizes(const QSize& min, const QSize& max);
    
signals:
    void statementFinished(const QString statement);
    
public slots:
    void hideWidget();

private slots:
    void finishOperationStatement();
    void loadScript();
    void saveScript();
    
private:
    std::unique_ptr<CustomTextEdit> mStatementEditor;
    std::unique_ptr<CustomTextEdit> mErrorLog;
    std::unique_ptr<QPushButton> mLoadScriptButton;
    std::unique_ptr<QPushButton> mSaveScriptButton;
    std::unique_ptr<QPushButton> mApplyButton;
    std::unique_ptr<QPushButton> mCancelButton;
    std::unique_ptr<QVBoxLayout> mLayout;
};

#endif
