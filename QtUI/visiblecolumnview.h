#ifndef VISIBLECOLUMNVIEW_H
#define VISIBLECOLUMNVIEW_H

#include <QTableView>

class QMouseEvent;

class VisibleColumnView : public QTableView
{
    Q_OBJECT
    
public:
    VisibleColumnView(QWidget *parent = Q_NULLPTR);
    ~VisibleColumnView(){}

public slots:
    void clearSelection();
    
protected:
    void mousePressEvent(QMouseEvent* event) override;
};

#endif
