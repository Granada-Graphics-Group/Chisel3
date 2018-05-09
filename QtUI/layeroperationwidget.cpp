#include "layeroperationwidget.h"

#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QApplication>
#include <QDebug>
#include <fstream>

LayerOperationWidget::LayerOperationWidget(QWidget* parent)
:
    BalloonWidget(parent)
{
    setTipOrientation(TipOrientation::Left);
    setWindowModality(Qt::NonModal);
    setSizePolicy({ QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding });
    
    mStatementEditor = std::make_unique<CustomTextEdit>(this);    
    //mStatementEditor->setSizePolicy({ QSizePolicy::Fixed, QSizePolicy::Fixed });
    mStatementEditor->setWordWrapMode(QTextOption::NoWrap);
    mStatementEditor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //mStatementEditor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mStatementEditor->setFrameStyle(QFrame::NoFrame);

    mLoadScriptButton = std::make_unique<QPushButton>("Load Script", this);
    mLoadScriptButton->setObjectName(QStringLiteral("Load Script Button"));
    mLoadScriptButton->setMaximumHeight(mLoadScriptButton->fontMetrics().height() + 6);
    mLoadScriptButton->setDefault(true);
    
    mSaveScriptButton = std::make_unique<QPushButton>("Save Script", this);
    mSaveScriptButton->setObjectName(QStringLiteral("Save Script Button"));
    mSaveScriptButton->setMaximumHeight(mSaveScriptButton->fontMetrics().height() + 6);
    mSaveScriptButton->setDefault(true);    
    
    mApplyButton = std::make_unique<QPushButton>("Apply", this);
    mApplyButton->setObjectName(QStringLiteral("Apply Button"));
    mApplyButton->setMaximumHeight(mApplyButton->fontMetrics().height() + 6);
    mApplyButton->setDefault(true);
    
    mCancelButton = std::make_unique<QPushButton>("Cancel", this);
    mCancelButton->setObjectName(QStringLiteral("Cancel Button"));
    mCancelButton->setMaximumHeight(mApplyButton->fontMetrics().height() + 6);
    mCancelButton->setDefault(false);

    //mDialogButtonBox = std::make_unique<QDialogButtonBox>(this);
    QDialogButtonBox* mDialogButtonBox = new QDialogButtonBox(this);
    mDialogButtonBox->addButton(mLoadScriptButton.get(), QDialogButtonBox::ActionRole);
    mDialogButtonBox->addButton(mSaveScriptButton.get(), QDialogButtonBox::ActionRole);
    mDialogButtonBox->addButton(mApplyButton.get(), QDialogButtonBox::ApplyRole);
    mDialogButtonBox->addButton(mCancelButton.get(), QDialogButtonBox::RejectRole);
  
    mErrorLog = std::make_unique<CustomTextEdit>(this);
    //mErrorLog->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mErrorLog->setWordWrapMode(QTextOption::NoWrap);
    mErrorLog->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //mErrorLog->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mErrorLog->setFrameStyle(QFrame::NoFrame);
    mErrorLog->setReadOnly(true);
    mErrorLog->hide();
    
    mLayout = std::make_unique<QVBoxLayout>();
    mLayout->setObjectName(QStringLiteral("VLayout"));
    mLayout->setSizeConstraint(QLayout::SetFixedSize);
    mLayout->setContentsMargins(tipHeight() + 5, 5, 5, 5);
    mLayout->addWidget(mStatementEditor.get());    
    mLayout->addWidget(mErrorLog.get());
    mLayout->addWidget(mDialogButtonBox);    
    
    setLayout(mLayout.get());
    
    connect(mLoadScriptButton.get(), &QPushButton::released, this, &LayerOperationWidget::loadScript);
    connect(mSaveScriptButton.get(), &QPushButton::released, this, &LayerOperationWidget::saveScript);
    connect(mApplyButton.get(), &QPushButton::released, this, &LayerOperationWidget::finishOperationStatement);
    connect(mCancelButton.get(), &QPushButton::released, this, &LayerOperationWidget::hideWidget);
}


// *** Public Methods *** //

void LayerOperationWidget::setEditorText(const std::string& text)
{
    mStatementEditor->setHtml(QString::fromStdString(text));
}

void LayerOperationWidget::setErrorLogText(const std::string& text)
{    
    if(mErrorLog->isHidden()) mErrorLog->show();
    mErrorLog->setText(QString::fromStdString(text));
}

void LayerOperationWidget::setEditorFocus()
{
    mStatementEditor->setFocus();
}

void LayerOperationWidget::setMaxAndMinSizes(const QSize& min, const QSize& max)
{
    setMinimumSize(min);
    setMaximumSize(max);
    
    auto margins = mLayout->contentsMargins();    
    mStatementEditor->setMaximumSize(max.width() - (margins.left() + margins.right() + 2), 4 * (max.height() - (pos().y() + mApplyButton->height() + margins.bottom() + margins.top()))/5);
    mStatementEditor->setMinimumWidth(minimumWidth() - (margins.left() + margins.right()));
        
    mErrorLog->setMaximumSize(max.width() - (margins.left() + margins.right() + 2), (max.height() - (pos().y() + mApplyButton->height() + margins.bottom() + margins.top()))/5);
    mErrorLog->setMinimumWidth(minimumWidth() - (margins.left() + margins.right()));
}


// *** Public slots *** //

void LayerOperationWidget::hideWidget()
{
    mStatementEditor->clear();
    mErrorLog->clear();
    
    mStatementEditor->resize(mStatementEditor->minimumSize());
    mErrorLog->resize(mErrorLog->minimumSize());
    resize(minimumSize());    
    
    mErrorLog->hide();
    hide();
}



// *** Private slots *** //

void LayerOperationWidget::finishOperationStatement()
{
    emit statementFinished(mStatementEditor->toPlainText());
}

void LayerOperationWidget::loadScript()
{
    auto filepath = QFileDialog::getOpenFileName(this, tr("Load Scripting File"), QApplication::applicationDirPath(), tr("Chisel Scripting Files (*.chs)"));
    QFileInfo file(filepath);
    
    if(file.exists())
    {
        std::ifstream scriptStream(filepath.toStdString());
        
        if(scriptStream.good())
        {
            std::string scriptString;

            scriptStream.seekg(0, std::ios::end);   
            scriptString.reserve(scriptStream.tellg());
            scriptStream.seekg(0, std::ios::beg);

            scriptString.assign((std::istreambuf_iterator<char>(scriptStream)), std::istreambuf_iterator<char>());
            
            mStatementEditor->setText(QString::fromStdString(scriptString));
        }
    }
}

void LayerOperationWidget::saveScript()
{
    auto filepath = QFileDialog::getSaveFileName(this, tr("Save Script"), QApplication::applicationDirPath(), tr("Chisel Scripting Files (*.chs)"));
    
    if(filepath.length())
    {
        std::ofstream scriptStream(filepath.toStdString());        
        scriptStream << mStatementEditor->toPlainText().toStdString();
        scriptStream.close();
    }
}
