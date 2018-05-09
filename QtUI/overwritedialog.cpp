#include "overwritedialog.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

OverwriteDialog::OverwriteDialog(const QFileInfo& source, const QFileInfo& destination, const QStringList& resourceList, std::string resource, QWidget* parent)
:
    QDialog(parent),
    mSource(source),
    mDestination(destination),
    mResourceList(resourceList)
{   
    setWindowTitle("The " + QString(resource.c_str()) + " already exists");

    QLabel* textLabel = new QLabel(this);
    textLabel->setText("Do you want to overwrite the existing " + QString(resource.c_str()) + "\n\n    " 
                            + mDestination.fileName() + "\n    Size: " 
                            + QString::number(mDestination.size()) + " bytes\n    Date Modified: " + mDestination.lastModified().toString()
                            + "\n\nwith this one?\n\n    " + mSource.fileName() + "\n    Size: " + QString::number(mSource.size()) 
                            + " bytes\n    Date Modified: " + mSource.lastModified().toString() + "\n\n"
                        );
    
    mNewNameEdit = new QLineEdit(this);
    mNewNameEdit->setObjectName(QStringLiteral("newNameEdit"));
    mNewNameEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mNewNameEdit->setText(mSource.fileName());
    QRegularExpression rx("^[a-zA-Z0-9][a-zA-Z0-9 ._-]*$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    mNewNameEdit->setValidator(validator);
    
    
    mNewNameButton = new QPushButton("Suggest New Name", this);
    mNewNameButton->setObjectName(QStringLiteral("newNameButton"));
    
    QHBoxLayout* newNameLayout = new QHBoxLayout();
    newNameLayout->setObjectName(QStringLiteral("newNameLayout"));    
    newNameLayout->addWidget(mNewNameEdit);
    newNameLayout->addSpacing(6);
    newNameLayout->addWidget(mNewNameButton);
    
    mRenameButton = new QPushButton("Rename", this);
    mRenameButton->setObjectName(QStringLiteral("rename"));
    mRenameButton->setDisabled(true);

    mOverwriteButton = new QPushButton("Overwrite", this);
    mOverwriteButton->setObjectName(QStringLiteral("overwrite"));
    
    mSkipButton = new QPushButton("Skip", this);
    mSkipButton->setObjectName(QStringLiteral("skip"));

    mCancelButton = new QPushButton("Cancel", this);
    mCancelButton->setObjectName(QStringLiteral("cancel"));

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setObjectName(QStringLiteral("buttonLayout"));
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(mRenameButton);
    buttonLayout->addWidget(mOverwriteButton);
    buttonLayout->addWidget(mSkipButton);
    buttonLayout->addWidget(mCancelButton);
        
    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->setObjectName(QStringLiteral("buttonLayout"));
    globalLayout->addWidget(textLabel);
    globalLayout->addStretch(1);
    globalLayout->addItem(newNameLayout);
    globalLayout->addItem(buttonLayout);
    
    setLayout(globalLayout);

    connect(mNewNameButton, &QPushButton::released, this, &OverwriteDialog::renameSource);
    connect(mNewNameEdit, &QLineEdit::editingFinished, this, &OverwriteDialog::renameSource);
    connect(mRenameButton, &QPushButton::released, this, &QDialog::accept);
    connect(mOverwriteButton, &QPushButton::released, this, &QDialog::accept);
    connect(mSkipButton, &QPushButton::released, this, &OverwriteDialog::skip);
    connect(mCancelButton, &QPushButton::released, this, &QDialog::reject);
}


// *** Private Slots *** //

void OverwriteDialog::renameSource()
{            
    if(!mResourceList.contains(mNewNameEdit->text()))
    {
        value = 1;
        mRenameButton->setEnabled(true);
    }
    else
        mRenameButton->setDisabled(true);
}

void OverwriteDialog::skip()
{
    value = -1;
    accept();
}
