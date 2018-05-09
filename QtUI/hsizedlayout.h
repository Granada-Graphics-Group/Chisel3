#ifndef HSIZEDLAYOUT_H
#define HSIZEDLAYOUT_H

#include <QWidget>
#include <QHBoxLayout>

class HSizedLayout : public QHBoxLayout
{
    Q_OBJECT
    
public:
    HSizedLayout(QSize maximum, QWidget* parent = Q_NULLPTR);
    
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize maximumSize() const Q_DECL_OVERRIDE;
private:
    QSize mMaximumSize;

};


#endif

