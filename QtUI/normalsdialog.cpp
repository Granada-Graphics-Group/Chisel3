#include "normalsdialog.h"


NormalsDialog::NormalsDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Normals");
    
    mUi->functionLayerWidget->hide();
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->baseLayerWidget->hide();
    mUi->radiusWidget->hide();
    adjustSize();

    setValidLayerName("Normals");    
}


// *** Public Slots *** //

void NormalsDialog::accept()
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
        
        emit normals(   mUi->nameTextEdit->text().toStdString(),
                        resolution                        
                    );
        
        QDialog::accept();
    }    
}

