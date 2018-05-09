#ifndef DATABASERESOURCEVIEW_H
#define DATABASERESOURCEVIEW_H

#include <QTreeView>

class DataBaseResourceModel;

class DataBaseResourceView : public QTreeView
{
    Q_OBJECT

public:
    DataBaseResourceView(DataBaseResourceModel* model, QWidget *parent = Q_NULLPTR);
    
public slots:
    void updateViewModelData();
    
protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

};

#endif

