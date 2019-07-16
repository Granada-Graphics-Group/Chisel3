#ifndef OPERATIONDIALOG_H
#define OPERATIONDIALOG_H

#include "ui_operationdialog.h"
#include "balloonmessagedialog.h"

#include <memory>

namespace Ui 
{
    class ComputeLayerDialog;
}

class ResourceManager;
class Chisel;

class OperationDialog : public QDialog
{
    Q_OBJECT
    
public:
    OperationDialog(Chisel* chisel, QWidget *parent = 0): OperationDialog("", chisel, parent) {}
    OperationDialog(std::string layerName, Chisel* chisel, QWidget *parent = 0);

// public slots:
//     virtual void accept() override;
    
protected slots:
    void validateLayerName();
    void updateDBNumericFields(int layerIndex);
    void filterLayersByResolution(int resolutionIndex);
    void updateExecuteButtonState();
    void updateFieldWidgetVisibility();
    
protected:    
    void moveEvent(QMoveEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void showEvent(QShowEvent * event) override;
    bool isNameValid() const { return mValidName; }
    void setValidLayerName(std::string name); 

    std::unique_ptr<Ui::OperationDialog> mUi;

private:    
    std::unique_ptr<BalloonMessageDialog> mErrorDialog;
    
    Chisel* mChisel;
    ResourceManager* mManager;
    bool mValidName = false;   
};

#endif
