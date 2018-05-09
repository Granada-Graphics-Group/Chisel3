#include "databasetableschemadelegate.h"

#include <QComboBox>
#include <QPainter>
#include <QApplication>
#include <QAbstractItemView>

DataBaseTableSchemaDelegate::DataBaseTableSchemaDelegate(const QStringList& items, QObject* parent)
: 
    QStyledItemDelegate(parent),
    mItems(items)
{

}

DataBaseTableSchemaDelegate::~DataBaseTableSchemaDelegate()
{

}


// *** Public Methods *** //

// void DataBaseTableDefinitionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
// {   
//     auto itemIndex = qvariant_cast<unsigned int>(index.data(Qt::UserRole + 1));    
//     painter->drawText(option.rect, mItems[itemIndex]);  
// }

QSize DataBaseTableSchemaDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto tempSize = QStyledItemDelegate::sizeHint(option, index);
    tempSize.setWidth(tempSize.width() + 15);
    
    return tempSize;
}

QWidget* DataBaseTableSchemaDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QComboBox* editor = new QComboBox(parent);
    
    for(auto item: mItems)
        editor->addItem(item);
    
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

void DataBaseTableSchemaDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::UserRole).toUInt();
    comboBox->setCurrentIndex(value);    
}

void DataBaseTableSchemaDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentIndex(), Qt::EditRole);    
}

void DataBaseTableSchemaDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}
