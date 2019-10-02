#include "databasetabledatadelegate.h"
#include "databasetable.h"
#include "customspinbox.h"

#include <QDateEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QAbstractItemView>
#include <QApplication>
#include <QIntValidator>
#include <QDoubleValidator>
#include <limits>



Q_DECLARE_METATYPE(DataBaseField::Type)

DataBaseTableDataDelegate::DataBaseTableDataDelegate(QObject* parent)
:
    QStyledItemDelegate(parent)
{

}

DataBaseTableDataDelegate::~DataBaseTableDataDelegate()
{

}

// *** Public Methods *** //

QWidget* DataBaseTableDataDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if(index.column() != 0)
    {
        auto type = qvariant_cast<DataBaseField::Type>(index.model()->data(index, Qt::UserRole));

        switch(type)
        {
            case DataBaseField::Type::Integer:
            {
                QSpinBox* editor = new QSpinBox(parent);
                editor->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
                
                return editor;
            }
            case DataBaseField::Type::Double:
            {
                QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
                editor->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<double>::max());
                editor->setDecimals(6);
                
                return editor;
            }        
            case DataBaseField::Type::Date:
            {
                QDateEdit* editor = new QDateEdit(QDate::currentDate(), parent);
    // //             editor->setDateTime(QDateTime::fromString(index.data().toString(), "dd/MM/yyyy hh:mm:ss"));
    // //             editor->setDisplayFormat("dd/mm/yyyy");
                editor->setCalendarPopup(true);
                
                return editor;
            }
            case DataBaseField::Type::String:
            {
                QLineEdit* editor = new QLineEdit(parent);
                //editor->setValidator(new QDoubleValidator(editor));
                
                return editor;
            }
            case DataBaseField::Type::Resource:
            {
                auto resources = qvariant_cast<QStringList>(index.model()->data(index, Qt::UserRole + 1));
                resources.prepend("");
                
                QComboBox* editor = new QComboBox(parent);
                editor->setEditable(false);
                editor->addItems(resources);
                
                int scrollbarWidth = editor->count() <= editor->maxVisibleItems() ? 0 : QApplication::style()->pixelMetric(QStyle::PixelMetric::PM_ScrollBarExtent);
                int IconWidth = editor->iconSize().width();

                int max = 0;

                for (int i = 0; i < editor->count(); i++)
                {
                    int width = editor->view()->fontMetrics().width(editor->itemText(i));
                    if (max < width)max = width;
                }

                editor->view()->setMinimumWidth(scrollbarWidth + IconWidth + max);

                return editor;
            }
        }
    }
    else
    {
        auto ids = qvariant_cast<QList<int>>(index.model()->data(index, Qt::UserRole));
        
        CustomSpinBox* editor = new CustomSpinBox(ids, parent);        
        editor->setRange(0, std::numeric_limits<int>::max());                
        
        return editor;        
    }
}

void DataBaseTableDataDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto type = qvariant_cast<DataBaseField::Type>(index.model()->data(index, Qt::UserRole));

    if(index.column() != 0)
    {
        switch(type)
        {   
            case DataBaseField::Type::Integer:
            {
                QSpinBox* intEditor = static_cast<QSpinBox*>(editor);
                intEditor->setValue(index.model()->data(index, Qt::EditRole).toInt());
            }
            break;
            case DataBaseField::Type::Double:
            {
                QDoubleSpinBox* doubleEditor = static_cast<QDoubleSpinBox*>(editor);
                doubleEditor->setValue(index.model()->data(index, Qt::EditRole).toDouble());
            }
            break;
            case DataBaseField::Type::Date:
            {
                QDateEdit* dateEditor = static_cast<QDateEdit *>(editor);
                if(index.data().toString().length())
                    dateEditor->setDate(QDate::fromString(index.data().toString(), "dd/MM/yyyy"));
                else
                    dateEditor->setDate(QDate::currentDate());            
            }
            break;
            case DataBaseField::Type::String:
            {
                QLineEdit* stringEditor = static_cast<QLineEdit *>(editor);
                stringEditor->setText(index.model()->data(index, Qt::EditRole).toString());
            }
            break;
            case DataBaseField::Type::Resource:
            {
                QComboBox* resourceEditor = static_cast<QComboBox*>(editor);
                resourceEditor->setCurrentText(index.data().toString());
            }
            break;
        }
    }
    else
    {
        CustomSpinBox* intEditor = static_cast<CustomSpinBox*>(editor);
        intEditor->setInitValue(index.model()->data(index, Qt::EditRole).toInt());         
    }

}

void DataBaseTableDataDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto type = qvariant_cast<DataBaseField::Type>(index.model()->data(index, Qt::UserRole));

    switch(type)
    {   
        case DataBaseField::Type::Integer:
        {
            QSpinBox* intEditor = static_cast<QSpinBox*>(editor);
            intEditor->interpretText();
            model->setData(index, intEditor->value(), Qt::EditRole);
        }
        break;
        case DataBaseField::Type::Double:
        {
            QDoubleSpinBox* doubleEditor = static_cast<QDoubleSpinBox*>(editor);
            doubleEditor->interpretText();
            model->setData(index, doubleEditor->value(), Qt::EditRole);
        }
        break;
        case DataBaseField::Type::Date:
        {
            QDateEdit *dateEditor = static_cast<QDateEdit *>(editor);
            model->setData(index, dateEditor->date().toString("dd/MM/yyyy"));
        }
        break;
        case DataBaseField::Type::String:
        {
            QLineEdit* stringEditor = static_cast<QLineEdit*>(editor);
            model->setData(index, stringEditor->text());
        }
        break;
        case DataBaseField::Type::Resource:
        {
            QComboBox* resourceEditor = static_cast<QComboBox*>(editor);
            model->setData(index, resourceEditor->currentText(), Qt::EditRole);
        }
        break;
    }
}

void DataBaseTableDataDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}
