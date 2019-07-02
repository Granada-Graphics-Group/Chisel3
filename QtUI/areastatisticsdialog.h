#ifndef AREASTATISTICSDIALOG_H
#define AREASTATISTICSDIALOG_H

#include "ui_areastatisticsdialog.h"

#include <memory>

namespace Ui 
{
    class AreaStatisticsDialog;
}

class ResourceManager;
class BalloonMessageDialog;

class AreaStatisticsDialog : public QDialog
{
    Q_OBJECT
    
public:
    AreaStatisticsDialog(ResourceManager* manager, QWidget *parent = 0);

public slots:
    void accept() override;
    
// signals:
//     void createNewLayerSignal(QString name, Layer::Type type, std::pair<int, int> resolution, int paletteIndex);

protected:    
    void moveEvent(QMoveEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;    
    
private slots:
    void validateLayerName();
    
private:
    std::unique_ptr<Ui::AreaStatisticsDialog> mUi;
    std::unique_ptr<BalloonMessageDialog> mErrorDialog;
    
    ResourceManager* mManager;
    bool mValidName = false;
    
//     std::vector<std::string> mLayerNames;
//     DiskLayerModel* mDiskLayerModel;
//     bool mValidName = false;    
};

#endif
