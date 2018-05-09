#include "customtextedit.h"

#include <QScrollBar>
#include <QDebug>

CustomTextEdit::CustomTextEdit(QWidget* parent)
:
    QTextEdit(parent)
{
    
    connect(this, &CustomTextEdit::textChanged, this, &CustomTextEdit::updateGeometry);
}


// *** Public Methods *** //

QSize CustomTextEdit::sizeHint() const
{    
    QSize hint = document()->size().toSize();
        
    if(!verticalScrollBar()->visibleRegion().isEmpty())
        hint += {verticalScrollBar()->width() + 2 * frameWidth(), 0};
        
    return hint;
}

QSize CustomTextEdit::minimumSizeHint() const
{    
    QFontMetrics metrics(document()->defaultFont());
    return { minimumSize().width(), metrics.height() };
}

