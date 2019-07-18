#include "resamplingdialog.h"

ResamplingDialog::ResamplingDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Resampling");    
    
    mUi->baseLayerWidget->hide();
    mUi->operationLayerComboBox->removeItem(mUi->operationLayerComboBox->count() - 1);
    mUi->operationLayerComboBox->removeItem(mUi->operationLayerComboBox->count() - 1);
    
    adjustSize();
    
    setValidLayerName("Resampled");
}


// *** Public Slots *** //

void ResamplingDialog::accept()
{
    if(isNameValid())
    {   
        emit resampling(mUi->nameTextEdit->text().toStdString(),                                 
                        mUi->functionLayerComboBox->currentIndex(),
                        (mUi->fieldComboBox->count() > 0) ? mUi->fieldComboBox->currentData().toInt() : -1,
                        mUi->radiusSpinBox->value(),
                        mUi->operationLayerComboBox->currentIndex()
                       );
        
        QDialog::accept();
    }     
}
