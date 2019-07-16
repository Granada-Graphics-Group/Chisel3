#ifndef ORIENTATIONDIALOG_H
#define ORIENTATIONDIALOG_H

#include "operationdialog.h"

#include <glm/glm.hpp>

class OrientationDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    OrientationDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    
signals:
    void orientation(std::string name, std::pair<int, int> resolution, glm::vec3 reference); 
};

#endif
