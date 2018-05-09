#ifndef DATABASETABLESCHEMAMODEL_H
#define DATABASETABLESCHEMAMODEL_H

#include "databasetable.h"

#include <QAbstractTableModel>

class RegisterLayer;


class DataBaseTableSchemaModel : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    DataBaseTableSchemaModel(QObject *parent = Q_NULLPTR);
    ~DataBaseTableSchemaModel();
    
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    
    const QStringList& fieldTypeString() const { return mFieldTypes; }
    DataBaseTable* schema() const;
    
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    
    //void setTableSchema(DataBaseTable* schema);
    void setDatabaseLayer(RegisterLayer* layer);
    void copySchemaContents(const DataBaseTable& schema);

signals:    
    void tableSchemaCreated(const std::string& SQLStatement);
    
public slots:
    void addField();
    void deleteField();
    void deleteField(int index);
    void clearSchema();
    void createTableSchemaStatement();
    
private:
    DataBaseTable* mTableSchema = nullptr;
    RegisterLayer* mLayer = nullptr;
    QStringList mFieldTypes = {"Integer", "Double", "String", "Date", "Resource", "Area"};
    QStringList mSQLTypes = {"INTEGER", "REAL", "TEXT", "TEXT", "TEXT", "REAL"};
};

#endif // DATABASETABLESCHEMAMODEL_H
