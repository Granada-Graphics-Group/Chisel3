#include "controlpointvalidator.h"
#include "palette.h"

#include <QLocale>

ControlPointValidator::ControlPointValidator(Palette* palette, QObject* parent)
:
    QValidator(parent),
    mPalette(palette)
{
    
}


// *** Public Methods *** //

QValidator::State ControlPointValidator::validate(QString& input, int& pos) const
{
    if(mPalette->type() == Palette::Type::Linear)
    {
        QLocale locale;
        bool okConversion;
        double value = locale.toDouble(input, &okConversion);
        
        auto controlPoints = mPalette->controlPoints();
        auto found = controlPoints.find(value);
        
        if(found != end(controlPoints))
        {
            emit validated();
            return QValidator::Acceptable;
        }
        else
        {
            emit rejected("The double is not a control point");
            return QValidator::Invalid;
        }
    }
    else
    {
        QLocale locale;
        bool okConversion;
        int value = locale.toInt(input, &okConversion);
        
        auto controlPoints = mPalette->controlPoints();
        auto found = controlPoints.find(value);
        
        if(found != end(controlPoints))
        {
            emit validated();
            return QValidator::Acceptable;
        }
        else
        {
            emit rejected("The double is not a control point");
            return QValidator::Invalid;
        }
    }
}
