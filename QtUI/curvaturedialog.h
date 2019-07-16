#ifndef CURVATUREDIALOG_H
#define CURVATUREDIALOG_H

#include "operationdialog.h"

class CurvatureDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    CurvatureDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void curvature(std::string name, std::pair<int, int> resolution, double radius); 
};

#endif
