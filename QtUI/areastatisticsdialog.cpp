#include "areastatisticsdialog.h"

AreaStatisticsDialog::AreaStatisticsDialog(Chisel* chisel, QWidget* parent)
:
    OperationDialog(chisel, parent)
{
    setWindowTitle("Area Statistics");
    
    mUi->nameWidget->hide();    
    mUi->radiusWidget->hide();
    
    adjustSize();
}


// *** Public slots *** //

void AreaStatisticsDialog::accept()
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
        
        emit areaStatistics(    mUi->functionLayerComboBox->currentData().toInt(),
                                (mUi->fieldComboBox->count() > 0) ? mUi->fieldComboBox->currentData().toInt() : -1,
                                mUi->baseLayerComboBox->currentData().toInt(),
                                mUi->operationLayerComboBox->currentIndex()
                           );
        
        QDialog::accept();
    }    
}
