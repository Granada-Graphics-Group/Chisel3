#include "customspinbox.h"
#include "logger.hpp"

#include <algorithm>

CustomSpinBox::CustomSpinBox(const QList<int>& usedValues, QWidget* parent)
:
    QSpinBox(parent),
    mUsedValues(usedValues)
{
    setKeyboardTracking(false);
    connect(this, static_cast<void (CustomSpinBox::*)(int)>(&CustomSpinBox::valueChanged), this, &CustomSpinBox::onValueChanged);
    
    std::sort(mUsedValues.begin(), mUsedValues.end());
}


// *** Public slots *** //

void CustomSpinBox::onValueChanged(int newVal)
{
    mLastValue = mCurrentValue;
    
    if(mUsedValues.contains(newVal) && newVal != mInitValue)
    {
        if(mLastValue < newVal)
        {
/*            LOG("lastValue < newVal: ", mLastValue, ", ", newVal);  */                      
            bool done = false;
            for(int i = newVal + 1; i < mUsedValues.last() && !done; ++i)
            {
                if(i == mInitValue || !std::binary_search(mUsedValues.begin(), mUsedValues.end(), i))
                {
                    mCurrentValue = i;
                    setValue(i);
                    done = true;
                }
            }
            
            if(!done)
            {
                mCurrentValue = mUsedValues.last() + 1;
                setValue(mUsedValues.last() + 1);
            }
        }
        else
        {
/*            LOG("lastValue >= newVal: ", mLastValue, ", ", newVal); */           
            bool done = false;
            for(int i = newVal - 1; i > mUsedValues.first() && !done; --i)
            {
                if(i == mInitValue || !std::binary_search(mUsedValues.begin(), mUsedValues.end(), i))
                {
                    mCurrentValue = i;
                    setValue(i);
                    done = true;
                }
            }
            
            if(!done)
            {
                mCurrentValue = mLastValue;
                setValue(mLastValue);
            }
        }
    }
    else if(newVal == mInitValue)
    {
//         LOG("newVal == InitValue: ", newVal, ", ", mInitValue);
        mCurrentValue = mInitValue;
        setValue(mInitValue);
    }
    else if(!mUsedValues.contains(newVal))
        mCurrentValue = newVal;
}

void CustomSpinBox::setInitValue(int val)
{
    mInitValue = val;
    setValue(val);
}

void CustomSpinBox::setValue(int val)
{
    mLastValue = mCurrentValue;
    mCurrentValue = val;
//     LOG("Set Value: ", mCurrentValue);
    QSpinBox::setValue(mCurrentValue);
}

