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
    OperationDialog(Chisel* chisel, QWidget* parent = 0): OperationDialog(chisel, true, parent) {}
    OperationDialog(Chisel* chisel, bool sameResolution = true, QWidget* parent = 0): OperationDialog("", chisel, sameResolution, parent) {}
    OperationDialog(std::string layerName, Chisel* chisel, bool sameResolution = true, QWidget* parent = 0);

// public slots:
//     virtual void accept() override;
    
protected slots:
    void validateTargetLayerName();
    void updateDBNumericFields(int functionLayerCBIndex);
    std::pair<int, int> targetLayerResolution();
    std::pair<int, int> functionLayerResolution();
    void setCheckNameValidity(bool check) { mCheckNameValidity = check; }
    void setFieldWidgetVisibility(bool visible) { mFieldWidgetVisibility = visible; if (!visible) mUi->fieldWidget->hide(); }
    void setSameResolutionFilter(bool same) { mSameResolutionFilter = same; }
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
    bool mCheckNameValidity = true;
    bool mFieldWidgetVisibility = true;
    bool mSameResolutionFilter = true;
    bool mValidName = false;   
};

#endif
