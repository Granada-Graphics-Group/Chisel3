#ifndef DATABASETABLESCHEMADELEGATE_H
#define DATABASETABLESCHEMADELEGATE_H

#include <QStyledItemDelegate>
#include <QStringList>

class DataBaseTableSchemaDelegate : public QStyledItemDelegate
{
public:
    DataBaseTableSchemaDelegate(const QStringList& items, QObject* parent = Q_NULLPTR);
    ~DataBaseTableSchemaDelegate();
    
//     void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    
private:
    QStringList mItems;
};

#endif // DATABASETABLESCHEMADELEGATE_H
