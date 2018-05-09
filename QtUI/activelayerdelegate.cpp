#include "activelayerdelegate.h"

#include <QLineEdit>

ActiveLayerDelegate::ActiveLayerDelegate(QObject* parent)
:
    QStyledItemDelegate(parent)
{
    
}

ActiveLayerDelegate::~ActiveLayerDelegate()
{
    
}


// *** Public Methods *** //

QWidget * ActiveLayerDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QLineEdit* editor = new QLineEdit(parent);
                
    return editor;    
}

void ActiveLayerDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QLineEdit* textEditor = static_cast<QLineEdit*>(editor);
    textEditor->setText(index.model()->data(index, Qt::EditRole).toString());        
}

void ActiveLayerDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QLineEdit* textEditor = static_cast<QLineEdit*>(editor);
    model->setData(index, textEditor->text(), Qt::EditRole);    
}

