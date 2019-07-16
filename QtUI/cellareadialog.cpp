#include "cellareadialog.h"

CellAreaDialog::CellAreaDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Cell Area");
    
    mUi->functionLayerWidget->hide();
    mUi->fieldWidget->hide();
    mUi->baseLayerWidget->hide();
    mUi->radiusWidget->hide();
    mUi->operationWidget->hide();
    
    adjustSize();
    
    setValidLayerName("CellArea");
}


// *** Public slots *** //

void CellAreaDialog::accept()
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
        
        emit cellArea(  mUi->nameTextEdit->text().toStdString(),
                        resolution
                     );
        
        QDialog::accept();
    }   
}


