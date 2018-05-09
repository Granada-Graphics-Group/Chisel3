#include "intvalidatorreporter.h"

IntValidatorReporter::IntValidatorReporter(int minimum, int maximum, QObject* parent)
:
    QIntValidator(minimum, maximum, parent)
{

}


// *** Public Methods *** //

QValidator::State IntValidatorReporter::validate(QString& input, int& pos) const
{
//     if(input.length() > 0)
//     {
        auto result = QIntValidator::validate(input, pos);
        
        if (result == Invalid)
            emit rejected("Write a valid integer");
        else if (result == Intermediate && (input.size() > 1 || (input.size() == 1 && input.at(0) != '-')))
            emit rejected("The integer is out of range");    
        else if (result == Acceptable)
            emit validated();
                    
        return result;
//     }
//     else
//         return Invalid;
}



