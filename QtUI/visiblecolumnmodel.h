#ifndef VISIBLECOLUMNMODEL_H
#define VISIBLECOLUMNMODEL_H

#include <QStandardItemModel>

class DataBaseTable;

class VisibleColumnModel : public QStandardItemModel
{
public:    
    VisibleColumnModel(int rows, int columns, QObject *parent = Q_NULLPTR);
    
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;    
    int primaryColumn() const { return mPrimaryColumn; }
    const std::vector<bool>& secondaryColumns() const { return mSecondaryColumns; }
    bool isSecondaryColumnSelected(int column) const { return mSecondaryColumns[column]; }
    
    void setTableSchema(DataBaseTable* schema) { mTableSchema = schema; }
    void setColumnCount(int columns); 
    void setColumns(int primary, const std::vector<bool>& secondaries);
    void setPrimary(int column);
    void setSecondaryColumns(const std::vector<bool>& columns);
    void setSecondaryColumn(int column);
    void addSecondaryColumn(int column);
    void removeSecondaryColumn(int column);
    void clearColumns();
    
private:
    int mPrimaryColumn;
    std::vector<bool> mSecondaryColumns;
    DataBaseTable* mTableSchema;
};

#endif
