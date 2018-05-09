#include "doublevalidatorreporter.h"

DoubleValidatorReporter::DoubleValidatorReporter(double maximum, double minimum, QObject* parent): 
    QDoubleValidator(parent)
{
    setRange(maximum, minimum, 8);
    setLocale(QLocale::C);
}



// *** Public Methods *** //

QValidator::State DoubleValidatorReporter::validate(QString& input, int& pos) const
{
    auto result = QDoubleValidator::validate(input, pos);
    
    if (result == Invalid)
        emit rejected("Write a valid float");
    else if (result == Intermediate && (input.size() > 1 || (input.size() == 1 && input.at(0) != '-' && input.at(0) != '.')))
        emit rejected("The float is out of range");
    else if (result == Acceptable)
        emit validated();
                
    return result;
}

