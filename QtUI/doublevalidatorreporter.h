#ifndef DOUBLEVALIDATORREPORTER_H
#define DOUBLEVALIDATORREPORTER_H

#include <QDoubleValidator>
#include <limits>

class DoubleValidatorReporter: public QDoubleValidator
{
    Q_OBJECT
    
public:
    DoubleValidatorReporter(QObject *parent = Q_NULLPTR): DoubleValidatorReporter(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), parent) {};
    DoubleValidatorReporter(double maximum, double minimum, QObject* parent = Q_NULLPTR);
    
    QValidator::State validate(QString &input, int &pos) const Q_DECL_OVERRIDE;

signals:
    void rejected(const QString& text) const;
    void validated() const;
};

#endif // DOUBLEVALIDATORREPORTER_H
