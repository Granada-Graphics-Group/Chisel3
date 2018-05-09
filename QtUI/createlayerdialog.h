#ifndef CREATELAYERDIALOG_H
#define CREATELAYERDIALOG_H

#include "layer.h"

#include <QDialog>
#include <memory>

namespace Ui 
{
    class CreateLayerDialog;
}

class BalloonMessageDialog;
class DiskLayerModel;

class CreateLayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateLayerDialog(DiskLayerModel* diskLayerModel, QWidget *parent = 0);
    ~CreateLayerDialog();

public slots:
    void accept() override;
    
signals:
    void createNewLayerSignal(QString name, Layer::Type type, std::pair<int, int> resolution, int paletteIndex);

protected:    
    void moveEvent(QMoveEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;    
    
private slots:
    void validateLayerName();
    
private:
    std::unique_ptr<Ui::CreateLayerDialog> mUi;
    std::unique_ptr<BalloonMessageDialog> mErrorDialog;
    
    std::vector<std::string> mLayerNames;
    DiskLayerModel* mDiskLayerModel;
    bool mValidName = false;
};

#endif // CREATELAYERDIALOG_H
