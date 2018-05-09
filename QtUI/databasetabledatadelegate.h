#ifndef DATABASETABLEDATADELEGATE_H
#define DATABASETABLEDATADELEGATE_H

#include <QStyledItemDelegate>

class DataBaseTableDataDelegate : public QStyledItemDelegate
{
public:
    DataBaseTableDataDelegate(QObject* parent = Q_NULLPTR);
    ~DataBaseTableDataDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

private:    
    
};

#endif // DATABASETABLEDATADELEGATE_H
