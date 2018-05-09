#ifndef COLUMNVISIBILITYWIDGET
#define COLUMNVISIBILITYWIDGET

#include <QWidget>
#include <vector>
#include <string>

class ColumnVisibilityModel;
class QPushButton;

class ColumnVisibilityWidget : public QWidget
{
    Q_OBJECT
    
public:
    ColumnVisibilityWidget(ColumnVisibilityModel* model, QWidget *parent = Q_NULLPTR);
    
signals:
    void visibilityChanged();
    
private:
    QPushButton* mApplyVisibilityButton;
};

#endif
