#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H

#include <QTableView>

class CustomTableView : public QTableView
{
public:
    CustomTableView(QWidget* parent = Q_NULLPTR);
    
    void copyAll();
    
protected:    
    void keyPressEvent(QKeyEvent * event) override;
    
//private:
    void copy();
    void paste();
};

#endif
