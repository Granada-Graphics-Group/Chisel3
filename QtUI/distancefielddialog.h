#ifndef DISTANCEFIELDDIALOG_H
#define DISTANCEFIELDDIALOG_H

#include "operationdialog.h"

class DistanceFieldDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    DistanceFieldDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void distanceField(std::string name, unsigned int functionLayerIndex, double radius); 
};

#endif
