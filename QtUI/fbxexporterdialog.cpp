#include "fbxexporterdialog.h"
#include "chisel.h"
#include "ui_fbxexporterdialog.h"

#include <QFileDialog>

FBXExporterDialog::FBXExporterDialog(Chisel* chisel, QWidget* parent)
:
    QDialog(parent),
    mChisel(chisel)    
{
    mUi = std::make_unique<Ui::FBXExporterDialog>();
    mUi->setupUi(this);
    
    for(const auto& layerNameAndIndex : mChisel->activeLayerNameAndIndices({Layer::Type::Register}))
        mUi->segmentationLayerComboBox->addItem(layerNameAndIndex.first.c_str(), layerNameAndIndex.second);
    
    connect(mUi->openSaveDialogButton, &QToolButton::released, this, &FBXExporterDialog::openSaveFileDialog);
    connect(mUi->filePathEdit, &QLineEdit::textChanged, this, &FBXExporterDialog::validateFilePath);
}

FBXExporterDialog::~FBXExporterDialog()
{
    
}



// *** Private Slots *** //

void FBXExporterDialog::exportModel()
{
    QFileInfo file(mUi->filePathEdit->text());
    auto segmentModel = mUi->segmentModelBox->isChecked();
    auto layerIndex = mUi->segmentationLayerComboBox->currentData();
	auto exportCamera = mUi->exportCameraCheck->isChecked();
    if(file.suffix().length() == 0) file = QFileInfo(mUi->filePathEdit->text() + ".fbx");
    mChisel->exportChiselModel(file.absoluteFilePath().toStdString(), file.suffix().toStdString(), layerIndex.toInt(), segmentModel && layerIndex.isValid(), exportCamera);
    
    accept();
}

void FBXExporterDialog::openSaveFileDialog()
{
    auto filepath = QFileDialog::getSaveFileName(this, tr("Save Chisel Model As"), QApplication::applicationDirPath() + "/" + QString::fromStdString(mChisel->chiselName()), tr("FBX (*.fbx)"));
    
    mUi->filePathEdit->setText(filepath);   
}

void FBXExporterDialog::validateFilePath()
{
//     auto valid = false;
//     
//     if(mUi->filePathEdit->text().size())
//     {
//         QFileInfo file(mUi->filePathEdit->text());
//         
//         if(file.isFile())
//             valid = true;            
//     }

    mUi->buttonExport->setEnabled(true);
}


