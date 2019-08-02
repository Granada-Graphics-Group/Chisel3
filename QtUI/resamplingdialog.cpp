#include "resamplingdialog.h"

ResamplingDialog::ResamplingDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Resampling");    

    mUi->functionLayerLabel->setText("Source");
    mUi->baseLayerWidget->hide();
    mUi->operationLayerLabel->setText("Filter");
    mUi->radiusWidget->hide();
    mUi->operationLayerComboBox->clear();
    mUi->operationLayerComboBox->addItem("Mean");
    
    
    adjustSize();
    
    setValidLayerName("Resampled");
    
    disconnect(mUi->resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ResamplingDialog::filterLayersByResolution);
    connect(mUi->resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ResamplingDialog::updateFilterVisibility);
}


// *** Public Slots *** //

void ResamplingDialog::accept()
{
    if(isNameValid())
    {   
        emit resampling(mUi->nameTextEdit->text().toStdString(),                                 
                        mUi->functionLayerComboBox->currentData().toInt(),
                        (mUi->fieldComboBox->count() > 0) ? mUi->fieldComboBox->currentData().toInt() : -1,
                        targetLayerResolution(),
                        mUi->operationLayerComboBox->currentIndex()
                       );
        
        QDialog::accept();
    }     
}

void ResamplingDialog::updateFilterVisibility()
{
    if(targetLayerResolution() < functionLayerResolution())
        mUi->operationWidget->show();
    else
        mUi->operationWidget->hide();
}
