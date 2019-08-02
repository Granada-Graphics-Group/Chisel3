#ifndef DISTANCEBANDDIALOG_H
#define DISTANCEBANDDIALOG_H

#include "operationdialog.h"

class DistanceBandDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    DistanceBandDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    void validateRange();
    
signals:
    void distanceBand(std::string name, unsigned int baseLayerIndex, double radius); 
};

#endif
