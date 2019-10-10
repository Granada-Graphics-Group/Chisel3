#include "distancefielddialog.h"

DistanceFieldDialog::DistanceFieldDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Distance Field");
    setFieldWidgetVisibility(false);
    
    mUi->functionLayerWidget->hide();
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->radiusLabel->setText("Max Distance");
    adjustSize();

    setValidLayerName("DistanceField");
}


// *** Public Slots *** //

void DistanceFieldDialog::accept()
{
    if(isNameValid())
    {   
        emit distanceField( mUi->nameTextEdit->text().toStdString(),
                            mUi->baseLayerComboBox->currentData().toInt(),
                            mUi->radiusSpinBox->value()
                        );
        
        QDialog::accept();
    }
}
