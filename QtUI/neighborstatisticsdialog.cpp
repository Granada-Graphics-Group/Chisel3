#include "neighborstatisticsdialog.h"

NeighborStatisticsDialog::NeighborStatisticsDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Neighborhood Statistics");    
    
    mUi->baseLayerWidget->hide();
    mUi->operationLayerComboBox->removeItem(mUi->operationLayerComboBox->count() - 1);
    mUi->operationLayerComboBox->removeItem(mUi->operationLayerComboBox->count() - 1);
    
    adjustSize();
    
    setValidLayerName("NeighborStats");
}


// *** Public Slots *** //

void NeighborStatisticsDialog::accept()
{
    if(isNameValid())
    {   
        emit neighborStatistics(mUi->nameTextEdit->text().toStdString(),                                 
                                mUi->functionLayerComboBox->currentData().toInt(),
                                (mUi->fieldComboBox->count() > 0) ? mUi->fieldComboBox->currentData().toInt() : -1,
                                mUi->radiusSpinBox->value(),
                                mUi->operationLayerComboBox->currentIndex()
                               );
        
        QDialog::accept();
    }     
}

