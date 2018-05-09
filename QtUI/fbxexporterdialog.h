#ifndef FBXEXPORTERDIALOG
#define FBXEXPORTERDIALOG

#include <QDialog>

#include <memory>

namespace Ui 
{
    class FBXExporterDialog;
}

class Chisel;

class FBXExporterDialog : public QDialog
{
    Q_OBJECT
    
public:
    FBXExporterDialog(Chisel* chisel, QWidget* parent = nullptr);
    ~FBXExporterDialog();
    
private slots:
    void exportModel();
    void openSaveFileDialog();
    void validateFilePath();
    
private:
    std::unique_ptr<Ui::FBXExporterDialog> mUi;    
    Chisel* mChisel;
    
};

#endif
