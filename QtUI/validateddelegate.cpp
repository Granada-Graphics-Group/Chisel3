#include "validateddelegate.h"
#include "doublevalidatorreporter.h"
#include "intvalidatorreporter.h"
#include "palettemodel.h"
#include "palette.h"

#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>


ValidatedDelegate::ValidatedDelegate(ValidatorType type, QObject* parent)
:
    QStyledItemDelegate(parent),
    mType(type)
{

}

ValidatedDelegate::~ValidatedDelegate()
{
    
}


// *** Public Methods *** //

QWidget* ValidatedDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    
    if(mType == ValidatorType::Double)
    {
        DoubleValidatorReporter* validator = new DoubleValidatorReporter();
        editor->setValidator(validator);
        connect(validator, &DoubleValidatorReporter::rejected, this, &ValidatedDelegate::rejectedValidation);
        connect(validator, &DoubleValidatorReporter::validated, this, &ValidatedDelegate::acceptedValidation);
    }
    else
    {
        IntValidatorReporter* validator = new IntValidatorReporter();
        editor->setValidator(validator);
        connect(validator, &IntValidatorReporter::rejected, this, &ValidatedDelegate::rejectedValidation);
        connect(validator, &IntValidatorReporter::validated, this, &ValidatedDelegate::acceptedValidation);        
    }
       
    return editor;
}

void ValidatedDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto value = index.model()->data(index, Qt::DisplayRole);
    auto line = static_cast<QLineEdit*>(editor);
    
    if(!std::isinf(value.toDouble()))
        line->setText(value.toString());
}

void ValidatedDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto line = static_cast<QLineEdit*>(editor);
    auto value = line->text();    
    auto paletteModel = static_cast<PaletteModel *>(model);
    
    if(value.length() == 0 && paletteModel->containInf())
        emit cancelEdit();
    else
    {
        if(!paletteModel->palette()->containControlPoint(value.toDouble()))
            paletteModel->setData(index, value);
        else
        {        
            if(paletteModel->data(index, Qt::DisplayRole) != value)
                emit duplicatedInput("Duplicated value");
        }
    }
}

// *** Public Slots *** //
void ValidatedDelegate::rejectedValidation(const QString& text)
{
    emit invalidInput(text);
}

void ValidatedDelegate::acceptedValidation()
{
    emit validInput();
}



// *** Protected Methods *** //

bool ValidatedDelegate::eventFilter(QObject* editor, QEvent* event)
{
    QWidget *weditor = qobject_cast<QWidget*>(editor);
    if (!weditor)
        return false;
    if (event->type() == QEvent::KeyPress)
    {
        switch (static_cast<QKeyEvent *>(event)->key()) 
        {
        case Qt::Key_Escape:
            emit cancelEdit();
        }        
    }
    
    return QStyledItemDelegate::eventFilter(editor, event);
}
