#ifndef BALLOONMESSAGEDIALOG_H
#define BALLOONMESSAGEDIALOG_H

#include "balloondialog.h"

#include <QLabel>

#include <memory>

class BalloonMessageDialog : public BalloonDialog
{
    Q_OBJECT
    
public:
    BalloonMessageDialog(QWidget *parent = Q_NULLPTR): BalloonMessageDialog("", parent){};    
    BalloonMessageDialog(const QString &message, QWidget *parent = Q_NULLPTR);
    
    ~BalloonMessageDialog();

    QSize sizeHint() const override;
    QLabel* message() const { return mMessage.get(); }
    bool isEmpty() const { return mMessage->text().isEmpty(); }
    
    void setMessage(const QString& message);
    void setMessage(const QString& message, const QColor& color);
    void clearMessage();
        
private:
    std::unique_ptr<QLabel> mMessage;
};

#endif // BALLOONMESSAGEDIALOG_H
