#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>

class CustomTextEdit : public QTextEdit
{
public:
    CustomTextEdit(QWidget* parent = Q_NULLPTR);
    
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    
private:    
};

#endif

