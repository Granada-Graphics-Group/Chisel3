#ifndef BALLOONVALUEDIALOG_H
#define BALLOONVALUEDIALOG_H

#include "balloonmessagedialog.h"

class DataBaseTableDataModel;
class DataBaseRowDataView;

class BalloonValueDialog : public BalloonMessageDialog
{
public:
    BalloonValueDialog(DataBaseTableDataModel* tableModel, QWidget *parent = Q_NULLPTR);
    
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void showValue(float value, const QColor& color);
    void showRowValue(int rowIndex);

private:
    void adjustGeometry(bool rowVisible);
    
    std::unique_ptr<DataBaseRowDataView> mTableRowView;
    bool mShowDataBaseRow = false;
};

#endif
