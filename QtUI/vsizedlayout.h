#ifndef VSIZEDLAYOUT_H
#define VSIZEDLAYOUT_H

#include <QWidget>
#include <QVBoxLayout>

class VSizedLayout : public QVBoxLayout
{
    Q_OBJECT
    
public:
    VSizedLayout(QSize maximum, QWidget* parent = Q_NULLPTR);
    
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize maximumSize() const Q_DECL_OVERRIDE;
private:
    QSize mMaximumSize;

};


#endif
