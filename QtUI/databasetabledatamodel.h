#ifndef DATABASETABLEDATAMODEL_H
#define DATABASETABLEDATAMODEL_H

#include <QAbstractItemModel>
#include <QSqlRelationalTableModel>
#include <vector>
#include <array>

class DataBaseTable;

class DataBaseTableDataModel : public QSqlRelationalTableModel
{
    Q_OBJECT
    
public:
    DataBaseTableDataModel(QObject* parent = Q_NULLPTR, QSqlDatabase db = QSqlDatabase());
    ~DataBaseTableDataModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    DataBaseTable* schema() const { return mTableSchema; }
    bool isHeaderVisible() const { return mHeaderVisible; }
    std::string resourceDir() const { return mResourceDir; }
    int id(int row) const;
    
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    void setDataBaseTable(const std::string& name);
    void setSchema(DataBaseTable* schema);
    void setResourceDir(std::string path) { mResourceDir = path; }
    void setResourceFiles(const std::vector<std::string>& resourceFiles);
    void setAreaFields(const std::vector<std::array<float, 2>>& values);
    void setIdFilter(const QString& id) { setFilter("id = \'" + id + + "\'"); }
    void setHeaderVisible(bool visible) { mHeaderVisible = visible; };
    void setEditable(bool editable) { mEditable = editable; }
        
    bool commitData();

public slots:
    void loadTable();
    void addEndRow();
    void addRow(int index = -1);
    void delRow(int index);
    
signals:
    void dataCommitted();
    void tableSchemaModified();

private:
    void createTable();
    void alterTable();
    
    DataBaseTable* mTableSchema = nullptr;
    bool mHeaderVisible = true;
    bool mEditable = true;
    
    QList<int> mUsedIds;
    std::vector<std::string> mFieldTypes = {"Integer", "Double", "String", "Date", "Resource", "Area"};
    std::vector<std::string> mSQLTypes = {"INTEGER", "REAL", "TEXT", "TEXT", "TEXT", "REAL"};

    std::string mResourceDir;
    QStringList mResourceFiles;
};

#endif // DATABASETABLEDATAMODEL_H
