#include "lightingdialog.h"
#include "ui_lightingdialog.h"

LightingDialog::LightingDialog(QWidget* parent)
:
    QDialog(parent)
{
    mUi = std::make_unique<Ui::LightingDialog>();
    mUi->setupUi(this);
    
    connect(mUi->specularComponentCheck, &QCheckBox::stateChanged, this, &LightingDialog::computeSpecularComponentState);
    connect(mUi->specularColorSlider, &QSlider::valueChanged, this, &LightingDialog::computeSpecularColor);
    connect(mUi->specularPowerSlider, &QSlider::valueChanged, this, &LightingDialog::computeSpecularPower);
}

LightingDialog::~LightingDialog()
{
    
}

// *** Public Methods *** //

void LightingDialog::setSpecularComponentState(bool state)
{
    mUi->specularComponentCheck->setChecked(state);
}

void LightingDialog::setSpecularColor(float component)
{
    mUi->specularColorSlider->setValue(static_cast<int>(component * 1000));
}

void LightingDialog::setSpecularPower(float power)
{
    mUi->specularPowerSlider->setValue(static_cast<int>(256 - power));
}

// *** Private slots *** //

void LightingDialog::computeSpecularComponentState()
{
    if(mUi->specularComponentCheck->checkState() == Qt::Unchecked)
        emit specularColorEdited(0.0);
    else if(mUi->specularComponentCheck->checkState() == Qt::Checked)
        computeSpecularColor();
}

void LightingDialog::computeSpecularColor()
{
    emit specularColorEdited(mUi->specularColorSlider->value()/1000.0f);
}

void LightingDialog::computeSpecularPower()
{
    emit specularPowerEdited(256 - mUi->specularPowerSlider->value());
}
