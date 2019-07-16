#include "roughnessdialog.h"


RoughnessDialog::RoughnessDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Roughness");
    
    mUi->functionLayerWidget->hide();
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->baseLayerWidget->hide();
    adjustSize();

    setValidLayerName("Roughness");    
}


// *** Public Slots *** //

void RoughnessDialog::accept()
{
    if(isNameValid())
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
        
        emit roughness( mUi->nameTextEdit->text().toStdString(),
                        resolution,
                        mUi->radiusSpinBox->value()
                      );
        
        QDialog::accept();
    }      
}

