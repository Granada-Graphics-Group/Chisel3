#include "areastatisticsdialog.h"
#include "resourcemanager.h"
#include "balloonmessagedialog.h"


AreaStatisticsDialog::AreaStatisticsDialog(ResourceManager* manager, QWidget* parent)
:
    QDialog(parent),
    mManager(manager)
{
    mUi = std::make_unique<Ui::AreaStatisticsDialog>();
    mUi->setupUi(this);
    
    mErrorDialog = std::make_unique<BalloonMessageDialog>(this);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setTipOrientation(BalloonDialog::TipOrientation::Left);    
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed);
    
    connect(mUi->nameTextEdit, &QLineEdit::textChanged, this, &AreaStatisticsDialog::validateLayerName);    
}


// *** Public slots *** //

void AreaStatisticsDialog::accept()
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

void AreaStatisticsDialog::moveEvent(QMoveEvent* event)
{
    QDialog::moveEvent(event);
    
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }
}

void AreaStatisticsDialog::mouseMoveEvent(QMouseEvent* event)
{
    QDialog::mouseMoveEvent(event);
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }    
}


// *** Private slots *** //

void AreaStatisticsDialog::validateLayerName()
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
