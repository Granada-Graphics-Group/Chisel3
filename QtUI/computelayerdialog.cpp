#include "computelayerdialog.h"
#include "ui_computelayerdialog.h"
#include "resourcemanager.h"
#include "balloonmessagedialog.h"


ComputeLayerDialog::ComputeLayerDialog(ResourceManager* manager, QWidget* parent)
:
    QDialog(parent),
    mManager(manager)
{
    mUi = std::make_unique<Ui::ComputeLayerDialog>();
    mUi->setupUi(this);
    
    mErrorDialog = std::make_unique<BalloonMessageDialog>(this);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setTipOrientation(BalloonDialog::TipOrientation::Left);    
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed);
    
    connect(mUi->nameTextEdit, &QLineEdit::textChanged, this, &ComputeLayerDialog::validateLayerName);    
}


// *** Public slots *** //

void ComputeLayerDialog::accept()
{
    if(mValidName)
    {   
        std::pair<int, int> resolution;
        
        switch(mUi->resolutionComboBox->currentIndex())
        {
            case 0:
                resolution = {2048, 2048};
                break;
            case 1:
                resolution = {4096, 4096};
                break;
        }
            
        //emit createNewLayerSignal(mUi->nameTextEdit->text(), type, resolution, -1);
        QDialog::accept();
    }    
}


// *** Protected Methods *** //

void ComputeLayerDialog::moveEvent(QMoveEvent* event)
{
    QDialog::moveEvent(event);
    
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }
}

void ComputeLayerDialog::mouseMoveEvent(QMouseEvent* event)
{
    QDialog::mouseMoveEvent(event);
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }    
}


// *** Private slots *** //

void ComputeLayerDialog::validateLayerName()
{
    auto candidate = mUi->nameTextEdit->text().toStdString();
    
    if(mManager->isValidName(candidate))
    {
        if(!mManager->layerExists(candidate))
        {
            if(mErrorDialog->isVisible()) mErrorDialog->hide();
            mErrorDialog->clearMessage();            
            mValidName = true;            
        }
        else
        {
            mErrorDialog->setMessage("The layer already exists", Qt::white);        
            auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
            mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
            mErrorDialog->adjustSize();
            mErrorDialog->show();             
        }
    }
    else
    {
        mValidName = false;

        mErrorDialog->setMessage("Invalid Layer Name", Qt::white);
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
        mErrorDialog->adjustSize();
        mErrorDialog->show();           
    }   
}
