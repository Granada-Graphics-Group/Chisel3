#ifndef CUSTOMSPINBOX_H
#define CUSTOMSPINBOX_H

#include <QSpinBox>

class CustomSpinBox : public QSpinBox
{
    Q_OBJECT
    
public:
    CustomSpinBox(const QList<int>& usedValues, QWidget* parent = Q_NULLPTR);

    void setInitValue(int val);
        
public slots:
    void onValueChanged(int newVal);
    void setValue(int val);
    
private:
    int mLastValue;
    int mCurrentValue;
    int mInitValue;
    QList<int> mUsedValues;    
};

#endif
