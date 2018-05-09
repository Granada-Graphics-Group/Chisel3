#include "opacityeditorwidget.h"
#include "balloonmessagedialog.h"

#include <QSlider>
#include <QLineEdit>
#include <QHBoxLayout>

OpacityEditorWidget::OpacityEditorWidget(unsigned int layerIndex, float opacity, QWidget* parent)
:
    BalloonWidget(parent),
    mLayerIndex(layerIndex)
{
    setTipOrientation(TipOrientation::Left);
    setWindowModality(Qt::NonModal);
    
    mSlider = std::make_unique<QSlider>(this);
    mSlider->setObjectName(QStringLiteral("OpacitySlider"));
    mSlider->setOrientation(Qt::Horizontal);
    mSlider->setRange(0, 1000000);
    mSlider->setTickInterval(50000);
  
    mEditor = std::make_unique<QLineEdit>(this);
    mEditor->setObjectName(QStringLiteral("OpacityEditor"));
    mEditor->setMaximumWidth(50);
    mEditor->setAlignment(Qt::AlignCenter);
    mEditor->setStyleSheet("background-color: white;");

    
    mErrorDialog = std::make_unique<BalloonMessageDialog>(qobject_cast< QWidget* >(this->parent()));
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkGray); 

    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horizontalLayout->setContentsMargins(tipHeight(), 0, 0, 0);
    horizontalLayout->addWidget(mSlider.get());
    horizontalLayout->addWidget(mEditor.get());    
    setLayout(horizontalLayout);

    connect(mEditor.get(), &QLineEdit::editingFinished, this, &OpacityEditorWidget::updateOpacityValueFromEditor);
    connect(mSlider.get(), &QSlider::valueChanged, this, &OpacityEditorWidget::updateOpacityValueFromSlider);
    
    setOpacity(opacity);
}

OpacityEditorWidget::~OpacityEditorWidget()
{

}


// *** Public Methods *** //

void OpacityEditorWidget::setLayer(unsigned int index, float opacity)
{
    mLayerIndex = index;
    setOpacity(opacity);
}


void OpacityEditorWidget::setOpacity(float opacity)
{
    mEditor->setText(QString::number(opacity,'g', 2));
    
    QSignalBlocker blocker(mSlider.get());    
    mSlider->setValue(static_cast<int>(opacity * 1000000));
    blocker.unblock();    
}


// *** Public Slots *** //

void OpacityEditorWidget::updateOpacityValueFromEditor()
{
    auto newOpacity = mEditor->text().toFloat();
    
    mSlider->setValue(static_cast<int>(newOpacity * 1000000));
    
    emit opacityUpdated(mLayerIndex, newOpacity);
}

void OpacityEditorWidget::updateOpacityValueFromSlider()
{
    auto newOpacity = mSlider->value() / 1000000.0;
    
    mEditor->setText(QString::number(newOpacity,'g', 2));
    
    emit opacityUpdated(mLayerIndex, newOpacity);
}

