#ifndef CELLAREADIALOG_H
#define CELLAREADIALOG_H

#include "operationdialog.h"

class CellAreaDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    CellAreaDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void cellArea(std::string name, std::pair<int, int> resolution); 
};

#endif
