#include "createlayerdialog.h"
#include "ui_createlayerdialog.h"
#include "balloonmessagedialog.h"
#include "disklayermodel.h"

CreateLayerDialog::CreateLayerDialog(DiskLayerModel* diskLayerModel, QWidget *parent) :
    QDialog(parent),
    mDiskLayerModel(diskLayerModel)
{
    mUi = std::make_unique<Ui::CreateLayerDialog>();
    mUi->setupUi(this);
    
    mErrorDialog = std::make_unique<BalloonMessageDialog>(this);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setTipOrientation(BalloonDialog::TipOrientation::Left);    
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed);
    
    connect(mUi->nameTextEdit, &QLineEdit::textChanged, this, &CreateLayerDialog::validateLayerName);

}

CreateLayerDialog::~CreateLayerDialog()
{

}

//*** Public slots ***//

void CreateLayerDialog::accept()
{
    if(mValidName)
    {   
        Layer::Type type;
        std::pair<int, int> resolution;
                
        if(mUi->mNumericTypeButton->isChecked())
        {
            switch(mUi->numericTypeComboBox->currentIndex())
            {
                case 0:
                    type = Layer::Type::Int8;
                    break;
                case 1:
                    type = Layer::Type::UInt8;
                    break;
                case 2:
                    type = Layer::Type::Int16;
                    break;
                case 3:
                    type = Layer::Type::UInt16;
                    break;
                case 4:
                    type = Layer::Type::Int32;
                    break;
                case 5:
                    type = Layer::Type::UInt32;
                    break;
                case 6:
                    type = Layer::Type::Float16;
                    break;
                case 7:
                    type = Layer::Type::Float32;
                    break;
            }
        }
        else
            type = Layer::Type::Register;
        
        switch(mUi->resolutionComboBox->currentIndex())
        {
            //case 0:
            //    resolution = {512, 512};
            //    break;
            //case 1:
            //    resolution = {1024, 1024};
            //    break;
            case 0:
                resolution = {2048, 2048};
                break;
            case 1:
                resolution = {4096, 4096};
                break;
            case 2:
                resolution = {8192, 8192};
                break;
        }
            
        emit createNewLayerSignal(mUi->nameTextEdit->text(), type, resolution, -1);
        QDialog::accept();
    }
}


// *** Protected Methods *** //

void CreateLayerDialog::moveEvent(QMoveEvent* event)
{
    QDialog::moveEvent(event);
    
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }
}

void CreateLayerDialog::mouseMoveEvent(QMouseEvent* event)
{
    QDialog::mouseMoveEvent(event);
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }    
}


// *** Private Slots *** //

void CreateLayerDialog::validateLayerName()
{
    auto filenames = mDiskLayerModel->filenames();
    auto found = std::find_if(std::begin(filenames), std::end(filenames), [&](const std::string& filename) { return (filename == mUi->nameTextEdit->text().toStdString()) ? true : false; });

    if(found == std::end(filenames))
    {
        QRegExp regExp("^[a-zA-Z][a-zA-Z0-9_]*$");
        QStringList forbiddenNameList;
        forbiddenNameList<<"CON"<<"PRN"<<"AUX"<<"NUL"<<"COM"<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"<<"COM7"<<"COM8"<<"COM9"<<"LPT"<<"LPT1"<<"LPT2"<<"LPT3"<<"LPT4"<<"LPT5"<<"LPT6"<<"LPT7"<<"LPT8"<<"LPT9";
        
        if(mUi->nameTextEdit->text().contains(regExp) && !forbiddenNameList.contains(mUi->nameTextEdit->text().toUpper()))
        {        
            if(mErrorDialog->isVisible()) mErrorDialog->hide();
            mErrorDialog->clearMessage();            
            mValidName = true;
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
    else
    {
        mValidName = false; 
        
        mErrorDialog->setMessage("The layer already exists", Qt::white);        
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
        mErrorDialog->adjustSize();
        mErrorDialog->show();        
    }
}


