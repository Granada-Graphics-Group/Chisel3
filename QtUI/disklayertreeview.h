#ifndef DISKLAYERTREEVIEW_H
#define DISKLAYERTREEVIEW_H

#include <QTreeView>
#include <QMouseEvent>


class DiskLayerTreeView : public QTreeView
{
    Q_OBJECT

public:
    DiskLayerTreeView(QWidget *parent = Q_NULLPTR);
    
    std::vector<std::string> selectedLayerNames();

public slots:    
    void updateViewWithModelData();

signals:
    void LayerSelected(const std::vector<std::string>& layerNames);
    
protected:    
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    
private:

};

#endif
