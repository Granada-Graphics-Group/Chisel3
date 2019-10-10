#include "distancebanddialog.h"


DistanceBandDialog::DistanceBandDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Distance Band");
    setFieldWidgetVisibility(false);
    
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->functionLayerWidget->hide();
    mUi->radiusLabel->setText("Max Distance");
    adjustSize();

    //connect(mUi->radiusSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &DistanceBandDialog::validateRange);
    //connect(mUi->radiusMaxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &DistanceBandDialog::validateRange);
    
    setValidLayerName("DistanceBand");    
}


// *** Public Slots *** //

void DistanceBandDialog::accept()
{
    if(isNameValid())
    {   
        emit distanceBand(  mUi->nameTextEdit->text().toStdString(),
                            mUi->baseLayerComboBox->currentData().toInt(),
                            mUi->radiusSpinBox->value()
                         );
        
        QDialog::accept();
    }
}

void DistanceBandDialog::validateRange()
{
    if(mUi->radiusSpinBox->value() <= mUi->radiusMaxSpinBox->value())
        mUi->buttonExecute->setEnabled(true);
    else
        mUi->buttonExecute->setDisabled(true);
}
