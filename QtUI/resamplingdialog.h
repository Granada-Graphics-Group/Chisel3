#ifndef RESAMPLINGDIALOG_H
#define RESAMPLINGDIALOG_H

#include "operationdialog.h"

#include <memory>

class ResourceManager;
class Chisel;
class BalloonMessageDialog;

class ResamplingDialog : public OperationDialog
{
    Q_OBJECT
    
public:
    ResamplingDialog(Chisel* chisel, QWidget *parent = 0);

public slots:
    void accept() override;
    void updateFilterVisibility();
    
signals:
    void resampling(std::string name, unsigned int layerIndex, unsigned int fieldIndex, std::pair<int, int> resolution, unsigned int operationIndex);  
};


#endif
