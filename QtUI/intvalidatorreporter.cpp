#include "intvalidatorreporter.h"

IntValidatorReporter::IntValidatorReporter(int minimum, int maximum, QObject* parent)
:
    QIntValidator(minimum, maximum, parent)
{

}


// *** Public Methods *** //

QValidator::State IntValidatorReporter::validate(QString& input, int& pos) const
{
        auto result = QIntValidator::validate(input, pos);

        if (result == Acceptable)
            emit validated();
        else if (result == Intermediate && ((input.size() > 1 && !input.contains('.')) || (input.size() == 1 && input.at(0) != '-')))
            emit rejected("The integer is out of range");
        else if (result == Invalid || (result == Intermediate && input.contains('.')))
            emit rejected("Write a valid integer");
                    
        return result;
}



