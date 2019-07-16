#include "distancebanddialog.h"


DistanceBandDialog::DistanceBandDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Distance Band");
    
    mUi->fieldWidget->hide();
    mUi->operationWidget->hide();
    mUi->baseLayerWidget->hide();
    mUi->commaLabel->show();
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
                            static_cast<unsigned int>(mUi->functionLayerComboBox->currentIndex()),
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
