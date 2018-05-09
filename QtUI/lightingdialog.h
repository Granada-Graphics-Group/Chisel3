#ifndef LIGHTINGDIALOG
#define LIGHTINGDIALOG

#include <QDialog>
#include <memory>

namespace Ui 
{
    class LightingDialog;
}

class LightingDialog : public QDialog
{
    Q_OBJECT

public:
    LightingDialog(QWidget *parent = 0);
    ~LightingDialog();

    void setSpecularComponentState(bool state);
    void setSpecularColor(float component);
    void setSpecularPower(float power);
    
signals:
    void specularColorEdited(float component);
    void specularPowerEdited(float power);
    
private slots:
    void computeSpecularComponentState();
    void computeSpecularColor();
    void computeSpecularPower();
    
private:
    std::unique_ptr<Ui::LightingDialog> mUi;    
};

#endif
