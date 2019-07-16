#ifndef ROUGHNESSDIALOG_H
#define ROUGHNESSDIALOG_H

#include "operationdialog.h"

class RoughnessDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    RoughnessDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void roughness(std::string name, std::pair<int, int> resolution, double radius); 
};

#endif
