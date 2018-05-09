#ifndef ACTIVELAYERTREEVIEW_H
#define ACTIVELAYERTREEVIEW_H

#include <QTreeView>
#include <memory>

class ActiveLayerTreeView : public QTreeView
{
    Q_OBJECT

public:
    ActiveLayerTreeView(QWidget *parent = Q_NULLPTR);
    ~ActiveLayerTreeView();
    
    void setupSectionSize();
    void setContextMenu(QMenu* menu) { mContextMenu = menu; }    

    void dropEvent(QDropEvent* event);

private slots:
    void showContextMenu(const QPoint& pos);

private:
    QMenu* mContextMenu;
};

#endif // ACTIVELAYERTREEVIEW_H
