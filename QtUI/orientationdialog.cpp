#include "orientationdialog.h"

OrientationDialog::OrientationDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Orientation");
    
    mUi->functionLayerWidget->hide();
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->baseLayerWidget->hide();
    mUi->radiusWidget->hide();    
    mUi->vectorWidget->show();
    adjustSize();

    setValidLayerName("Orientation");    
}


// *** Public Slots *** //

void OrientationDialog::accept()
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
        
        emit orientation(   mUi->nameTextEdit->text().toStdString(),
                            resolution,
                            {
                                static_cast<float>(mUi->vecXSpinBox->value()),
                                static_cast<float>(mUi->vecYSpinBox->value()),
                                static_cast<float>(mUi->vecZSpinBox->value())
                            }
                        );
        
        QDialog::accept();
    }      
}
