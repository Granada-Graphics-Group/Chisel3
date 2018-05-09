#include "balloonmessagedialog.h"

#include <QHBoxLayout>
#include <QVariant>

BalloonMessageDialog::BalloonMessageDialog(const QString &message, QWidget* parent)
:
    BalloonDialog(parent)
{
    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    
    mMessage = std::make_unique<QLabel>(this);
    mMessage->setObjectName(QStringLiteral("Message"));
    mMessage->setAlignment(Qt::AlignCenter);
    setMessage(message);
        
    horizontalLayout->addWidget(mMessage.get());
    setLayout(horizontalLayout);
}

BalloonMessageDialog::~BalloonMessageDialog()
{

}


// *** Public Methods *** //

QSize BalloonMessageDialog::sizeHint() const
{
    return QSize(30, 30);
}

void BalloonMessageDialog::setMessage(const QString& message)
{
    mMessage->setText(message);
    
    switch(tipOrientation())
    {
        case BalloonDialog::TipOrientation::Up:
            layout()->setContentsMargins(10, tipHeight() + 10, 10, 10);
            break;
        case BalloonDialog::TipOrientation::Left:
            layout()->setContentsMargins(10 + tipHeight(), 10, 10, 10);
            break;
        case BalloonDialog::TipOrientation::Right:
            layout()->setContentsMargins(10, 10, tipHeight() + 10, 10);
            break;
        case BalloonDialog::TipOrientation::Down:
            layout()->setContentsMargins(10, 10, 10, tipHeight() + 10);
            break;            
    }
    
    adjustSize();
}

void BalloonMessageDialog::setMessage(const QString& message, const QColor& color)
{
    auto colorCode = QVariant(color).toString();
    mMessage->setStyleSheet("QLabel {color :" + colorCode + "; }");
    setMessage(message);
}

void BalloonMessageDialog::clearMessage()
{
    mMessage->clear();
    adjustSize();
}


