#ifndef DATABASEROWDATAVIEW_H
#define DATABASEROWDATAVIEW_H

#include "databasetabledataview.h"

class DataBaseRowDataView : public DataBaseTableDataView
{
public:
    DataBaseRowDataView(DataBaseTableDataModel* dataModel, bool resizeToContents, QWidget* parent = Q_NULLPTR) : DataBaseTableDataView(dataModel, resizeToContents, parent){};
    
    void mouseDoubleClickEvent(QMouseEvent * event) override;

/*private:
    DataBaseResourceModel* resourceModel;   */ 
};

#endif
