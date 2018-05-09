#ifndef INTVALIDATORREPORTER_H
#define INTVALIDATORREPORTER_H

#include <QIntValidator>
#include <limits>

class IntValidatorReporter: public QIntValidator
{
    Q_OBJECT

public:
    IntValidatorReporter(QObject* parent = Q_NULLPTR): IntValidatorReporter(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), parent){};
    IntValidatorReporter(int minimum, int maximum, QObject* parent = Q_NULLPTR);
    
    QValidator::State validate(QString& input, int& pos) const Q_DECL_OVERRIDE;

signals:
    void rejected(const QString& text) const;
    void validated() const;        
};

#endif // INTVALIDATORREPORTER_H
