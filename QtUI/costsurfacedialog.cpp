#include "costsurfacedialog.h"


CostSurfaceDialog::CostSurfaceDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Cost Surface");    
    
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->functionLayerLabel->setText("Base Layer");
    mUi->baseLayerLabel->setText("Friction Layer"); 
    mUi->radiusLabel->setText("Maximum Cost");
    adjustSize();
    
    setValidLayerName("CostSurface");
}


// *** Public Slots *** //

void CostSurfaceDialog::accept()
{
    if(isNameValid())
    {   
        emit costSurface(   mUi->nameTextEdit->text().toStdString(),
                            static_cast<unsigned int>(mUi->functionLayerComboBox->currentIndex()),
                            static_cast<unsigned int>(mUi->baseLayerComboBox->currentIndex()),
                            mUi->radiusSpinBox->value()
                        );
        
        QDialog::accept();
    }       
}
