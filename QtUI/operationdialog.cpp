#include "operationdialog.h"
#include "chisel.h"
#include "resourcemanager.h"
#include "balloonmessagedialog.h"


OperationDialog::OperationDialog(std::string layerName, Chisel* chisel, QWidget* parent)
:
    QDialog(parent),
    mChisel(chisel),
    mManager(chisel->resourceManager())
{
    mUi = std::make_unique<Ui::OperationDialog>();
    mUi->setupUi(this);
    
    mUi->commaLabel->hide();
    mUi->radiusSpinBox->setRange(0, std::numeric_limits<double>::max());
    mUi->radiusMaxSpinBox->setRange(0, std::numeric_limits<double>::max());
    mUi->radiusMaxSpinBox->hide();
    
    mUi->vecXSpinBox->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    mUi->vecYSpinBox->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    mUi->vecZSpinBox->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    mUi->vectorWidget->hide();
    
    mErrorDialog = std::make_unique<BalloonMessageDialog>(this);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setTipOrientation(BalloonDialog::TipOrientation::Left);    
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed);

    connect(mUi->resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OperationDialog::filterLayersByResolution);
    connect(mUi->functionLayerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OperationDialog::updateDBNumericFields);
    connect(mUi->nameTextEdit, &QLineEdit::textChanged, this, &OperationDialog::validateLayerName);
    
    mUi->nameTextEdit->setText(layerName.c_str());
    
    if(mChisel->layers().size())
        filterLayersByResolution(mUi->resolutionComboBox->currentIndex());
}


// *** Public slots *** //

// void OperationDialog::accept()
// {   
// }


// *** Protected slots *** //

void OperationDialog::validateLayerName()
{
    if(mUi->nameWidget->isVisible())
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
                
                mValidName = false;
            }
        }
        else
        {
            mErrorDialog->setMessage("Invalid Layer Name", Qt::white);
            auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
            mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
            mErrorDialog->adjustSize();
            mErrorDialog->show();
            
            mValidName = false;
        }
    }
    
    updateExecuteButtonState();
}

void OperationDialog::updateDBNumericFields(int layerIndex)
{
    mUi->fieldComboBox->clear();
    
    if(mChisel->layer(layerIndex)->registerType())
    {                
        auto dbLayer = static_cast<const RegisterLayer*>(mChisel->layer(layerIndex));
        
        for(auto i = 0; i < dbLayer->tableSchema()->fields().size(); ++i)
        {
            auto field = dbLayer->tableSchema()->field(i);
            
            if(field.isDouble() || field.isInteger())                
                mUi->fieldComboBox->addItem(field.mName.c_str(), i);
        }
    }

    updateFieldWidgetVisibility();
}

void OperationDialog::filterLayersByResolution(int resolutionIndex)
{
    std::pair<int, int> resolution;
    
    switch(resolutionIndex)
    {
        case 0:
            resolution = {2048, 2048};
            break;
        case 1:
            resolution = {4096, 4096};
            break;
    }

    {
        QSignalBlocker blocker(mUi->functionLayerComboBox);
        
        mUi->functionLayerComboBox->clear();
        mUi->baseLayerComboBox->clear();
        
        auto layers = mChisel->layers();
        
        for(auto i = 0; i < layers.size(); ++i)
        {
            if(layers[i]->resolution() == resolution)
            {
                mUi->functionLayerComboBox->addItem(layers[i]->name().c_str(), i);
                mUi->baseLayerComboBox->addItem(layers[i]->name().c_str(), i);
            }
        }
    }
    
    if(mUi->functionLayerComboBox->count())
        updateDBNumericFields(mUi->functionLayerComboBox->currentIndex());

    updateExecuteButtonState();
}

void OperationDialog::updateExecuteButtonState()
{
    auto visible = mUi->functionLayerComboBox->isVisible();
    auto layerCount = mUi->functionLayerComboBox->count() > 0;
    
    if((mValidName && visible && layerCount) || (mValidName && !visible))
        mUi->buttonExecute->setEnabled(true);
    else
        mUi->buttonExecute->setDisabled(true); 
}

void OperationDialog::updateFieldWidgetVisibility()
{
    if (mUi->fieldComboBox->count() > 0)
    {
        if(mUi->fieldWidget->isHidden())
            mUi->fieldWidget->show();
    }
    else if (mUi->fieldWidget->isVisible())
        mUi->fieldWidget->hide();
}


// *** Protected Methods *** //

void OperationDialog::moveEvent(QMoveEvent* event)
{
    QDialog::moveEvent(event);
    
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }
}

void OperationDialog::mouseMoveEvent(QMouseEvent* event)
{
    QDialog::mouseMoveEvent(event);
    if(!mErrorDialog->isEmpty())
    {
        auto height = mUi->nameTextEdit->height() - mErrorDialog->height();
        mErrorDialog->move(mapToGlobal(mUi->nameTextEdit->pos() + QPoint(mUi->nameTextEdit->width(), height/2.0)));
    }    
}

void OperationDialog::showEvent(QShowEvent* event)
{
    updateExecuteButtonState();
    updateFieldWidgetVisibility();
}

void OperationDialog::setValidLayerName(std::string name)
{
    mValidName = true;
    mUi->nameTextEdit->setText(mManager->createValidLayerName(name).c_str());
}