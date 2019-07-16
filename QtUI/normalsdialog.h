#ifndef NORMALSDIALOG_H
#define NORMALSDIALOG_H

#include "operationdialog.h"

class NormalsDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    NormalsDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void normals(std::string name, std::pair<int, int> resolution); 
};

#endif
