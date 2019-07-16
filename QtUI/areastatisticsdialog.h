#ifndef AREASTATISTICSDIALOG_H
#define AREASTATISTICSDIALOG_H

#include "operationdialog.h"

class AreaStatisticsDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    AreaStatisticsDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void areaStatistics(unsigned int layerIndex, unsigned int fieldIndex, unsigned int baseLayer, unsigned int operationIndex); 
};

#endif
