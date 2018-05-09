#include "hsizedlayout.h"

HSizedLayout::HSizedLayout(QSize maximum, QWidget* parent)
:
    QHBoxLayout(parent),
    mMaximumSize(maximum)
{
    setSizeConstraint(QLayout::SetFixedSize);
}


// *** Public Methods *** //

QSize HSizedLayout::sizeHint() const
{
    auto hint = QHBoxLayout::sizeHint();
    if(hint.width() > maximumSize().width())
        hint.setWidth(maximumSize().width());
    if (hint.height() > maximumSize().height())
        hint.setHeight(maximumSize().height());
    return hint;
}

QSize HSizedLayout::maximumSize() const
{
    return mMaximumSize;
}

