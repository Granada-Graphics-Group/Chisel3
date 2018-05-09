#include "vsizedlayout.h"

VSizedLayout::VSizedLayout(QSize maximum, QWidget* parent)
:
    QVBoxLayout(parent),
    mMaximumSize(maximum)
{
    setSizeConstraint(QLayout::SetFixedSize);
}


// *** Public Methods *** //

QSize VSizedLayout::sizeHint() const
{
    auto hint = QVBoxLayout::sizeHint();
    if(hint.width() > maximumSize().width())
        hint.setWidth(maximumSize().width());
    if (hint.height() > maximumSize().height())
        hint.setHeight(maximumSize().height());
    return hint;
}

QSize VSizedLayout::maximumSize() const
{
    return mMaximumSize;
}
