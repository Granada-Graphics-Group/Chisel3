#ifndef NEIGHBORSTATISTICSDIALOG_H
#define NEIGHBORSTATISTICSDIALOG_H

#include "operationdialog.h"

#include <memory>

class ResourceManager;
class Chisel;
class BalloonMessageDialog;

class NeighborStatisticsDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    NeighborStatisticsDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void neighborStatistics(std::string name, unsigned int layerIndex, unsigned int fieldIndex, double radius, unsigned int operationIndex);  
};

#endif
