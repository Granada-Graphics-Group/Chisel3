#ifndef COSTSURFACEDIALOG_H
#define COSTSURFACEDIALOG_H

#include "operationdialog.h"

class CostSurfaceDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    CostSurfaceDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void costSurface(std::string name, unsigned int functionLayerIndex, unsigned int costLayerIndex, double radius); 
};

#endif
