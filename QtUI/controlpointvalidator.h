#ifndef CONTROLPOINTVALIDATOR_H
#define CONTROLPOINTVALIDATOR_H

#include <QValidator>

class Palette;

class ControlPointValidator : public QValidator
{
    Q_OBJECT
    
public:
    ControlPointValidator(Palette* palette, QObject* parent = Q_NULLPTR);
    
    QValidator::State validate(QString& input, int& pos) const override;

signals:
    void rejected(const QString& text) const;
    void validated() const;
    
private:
    Palette* mPalette;
};

#endif
