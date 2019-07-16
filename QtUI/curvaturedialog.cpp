#include "curvaturedialog.h"


CurvatureDialog::CurvatureDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Curvature");
    
    mUi->functionLayerWidget->hide();
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->baseLayerWidget->hide();
    adjustSize();

    setValidLayerName("Curvature");     
}


// *** Public Slots *** //

void CurvatureDialog::accept()
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
        
        emit curvature( mUi->nameTextEdit->text().toStdString(),
                        resolution,
                        mUi->radiusSpinBox->value()
                      );
        
        QDialog::accept();
    }      
}
