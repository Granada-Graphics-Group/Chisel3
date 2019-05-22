#include "databasetabledatamodel.h"
#include "databasetable.h"
#include "logger.hpp"

#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>

Q_DECLARE_METATYPE(DataBaseField::Type)

DataBaseTableDataModel::DataBaseTableDataModel(QObject* parent, QSqlDatabase db)
:
    QSqlRelationalTableModel(parent, db)
{

}

DataBaseTableDataModel::~DataBaseTableDataModel()
{

}

// *** Public Methods *** //

QVariant DataBaseTableDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return (mHeaderVisible) ? QSqlRelationalTableModel::headerData(section, orientation, role) : QVariant();
}


QVariant DataBaseTableDataModel::data(const QModelIndex& index, int role) const
{
    if(index.isValid() && role == Qt::UserRole)        
    {
        if(index.column() == 0)
        {
            QVariant ids;
            ids.setValue(mUsedIds);
            return ids;
        }
        else
        {
            QVariant type;
            type.setValue(mTableSchema->field(index.column() - 1).mType);
            return type;
        }
    }
    else if (index.isValid() && role == (Qt::UserRole + 1))
    {
        if (mTableSchema->field(index.column() - 1).mType == DataBaseField::Type::Resource)
            return QVariant(mResourceFiles);
        else
            return QVariant();
    }
    else
        return QSqlRelationalTableModel::data(index, role);
}

Qt::ItemFlags DataBaseTableDataModel::flags(const QModelIndex& index) const
{
//     if(mEditable)
//     {
//         if(index.column() > 0 && mTableSchema->field(index.column() - 1).isArea())
//             return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
//         else
//             return QSqlRelationalTableModel::flags(index);
//     }
//     else
//         return QSqlRelationalTableModel::flags(index);
    return (mEditable) ? QSqlRelationalTableModel::flags(index) : Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int DataBaseTableDataModel::id(int row) const
{
    return record(row).value(0).toInt();
}

bool DataBaseTableDataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{    
    auto newValue = value;
    
    if(role == Qt::EditRole && index.column() == 0)
    {
        if(index.row() < mUsedIds.size())
        {
            LOG("Replace index: ", index.row(), ", with ", value.toInt()); 
            
            mUsedIds.replace(index.row(), value.toInt());
            
            std::string values;
    
            for(auto value : mUsedIds)
                values += std::to_string(value) + " ";
    
            LOG("Used Ids: ", values);            
            //std::sort(mUsedIds.begin(), mUsedIds.end());
        }
        else
        {
            LOG("UsedIds size: ", mUsedIds.size(), ", index row: ", index.row());
            //auto maxId = std::max_element(mUsedIds.begin(), mUsedIds.end());
            newValue = QVariant((mUsedIds.size()) ? *std::max_element(mUsedIds.begin(), mUsedIds.end()) + 1 : 0);
            mUsedIds.append(newValue.toInt());
        }
    }
    
    return QSqlRelationalTableModel::setData(index, newValue, role);
}


void DataBaseTableDataModel::setDataBaseTable(const std::string& name)
{
    auto tableName = QString::fromStdString(name);
    
    mUsedIds.clear();
    auto available = QSqlDatabase::isDriverAvailable("QSQLITE");
    auto valid = database().isValid();
    auto open = database().isOpen();
    auto err = database().isOpenError();
    auto last = database().lastError();
    auto dbname = database().databaseName();
    auto conname = database().connectionName();
    
    if(open)
    {
        auto tables = database().tables(QSql::SystemTables);
        //auto views = database().tables(QSql::Views);
//         auto all = database().tables(QSql::AllTables);
//         auto nnn = database().tables(QSql::Tables);
        
        auto tsize = tables.size();
    }
    
    if(!database().tables().contains(tableName))
        createTable();
    else
    {
        if(mTableSchema->isDirty())
            alterTable();
        
        QSqlQuery idQuery("SELECT id FROM " + tableName);
            
        while (idQuery.next())
        {
            auto id = idQuery.value(0).toInt();
            mUsedIds.append(id);
        }
    }

    setTable(tableName);
    auto error = lastError();
    LOG("Error DataBase: ", lastError().text().toStdString());
        
    select();
        
    LOG("Error DataBase - select: ", lastError().text().toStdString());
    
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    emit headerDataChanged(Qt::Horizontal, 0, static_cast<int>(mTableSchema->fields().size()));
    emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
}

void DataBaseTableDataModel::setSchema(DataBaseTable* schema)
{
    mTableSchema = schema;    
    if(mTableSchema->fields().size()) 
        setDataBaseTable(mTableSchema->name());
    else
        clear();
    
    //emit headerDataChanged(Qt::Horizontal, 0, static_cast<int>(schema->fields().size()));
}

void DataBaseTableDataModel::setResourceFiles(const std::vector<std::string>& resourceFiles)
{
    mResourceFiles.clear();
    for (const auto& resource : resourceFiles)
        mResourceFiles.append(QString::fromStdString(resource));
}

void DataBaseTableDataModel::addDoubleField(const std::string fieldName, const std::vector<std::array<double, 2> >& values)
{
    auto index = fieldIndex(fieldName.data());
    
    if(index < 0)
    {
        mTableSchema->addField({fieldName, DataBaseField::Type::Double, 0});
        index = mTableSchema->fields().size();
    }
    
    loadTable();
    
    setFieldData(index, values);
}

void DataBaseTableDataModel::setFieldData(const std::string fieldName, const std::vector<std::array<double, 2> >& values)
{
    setFieldData(fieldIndex(fieldName.data()), values);
}

void DataBaseTableDataModel::setFieldData(unsigned int fieldIndex, const std::vector<std::array<double, 2> >& values)
{
    if(fieldIndex < (mTableSchema->fields().size() + 1))
    {
        for(auto i = 0; i < rowCount(); ++i)
        {
            auto id = record(i).value("Id").toInt();
            auto foundId = std::find_if(begin(values), end(values), [&id](const std::array<double, 2>& value) { return (id == static_cast<int>(value[0])) ? true : false;});

            if (foundId != end(values))
                setData(index(i, fieldIndex), foundId->at(1));
            else
                setData(index(i, fieldIndex), "");
        }
        
        commitData();
    }
}

void DataBaseTableDataModel::setAreaFields(const std::vector<std::array<float, 2> >& values)
{
    std::vector<int> indexes;
    auto fields = mTableSchema->fields();
    
    for(auto i = 0; i < fields.size(); ++i)
        if(fields[i].isArea())
            indexes.push_back(i + 1);
    
    if(indexes.size())
    {
        for(auto i = 0; i < rowCount(); ++i)
            for (auto j = 0; j < indexes.size(); ++j)
            {
                auto id = record(i).value("Id").toInt();
                auto foundId = std::find_if(begin(values), end(values), [&id](const std::array<float, 2>& value) { return (id == static_cast<int>(value[0])) ? true : false; });

                if (foundId != end(values))
                    setData(index(i, indexes[j]), foundId->at(1));
                else                
                    setData(index(i, indexes[j]), "");
            }
            
        commitData();
    }        
}

bool DataBaseTableDataModel::commitData()
{
    database().transaction();
    
    if(isDirty())
    {
        if (submitAll())
        {
            database().commit();
            std::sort(mUsedIds.begin(), mUsedIds.end());
            emit dataCommitted();
            return true;
        }
        else
            return false;
    }
    else
        return false;
}


// *** Public Slots *** //

void DataBaseTableDataModel::loadTable()
{
    setDataBaseTable(mTableSchema->name());
}

void DataBaseTableDataModel::addEndRow()
{
    insertRecord(-1, record());    
}


void DataBaseTableDataModel::addRow(int index)
{
    int rowIndex = index;
    if(index >= rowCount()) rowIndex = -1;
    
//     auto newRecord = record();
//     newRecord.setValue(0, index);
    
    insertRecord(rowIndex, record());
    
    //commitData();
}


void DataBaseTableDataModel::delRow(int index)
{
    auto row = record(index);
    
    removeRow(index);
    
    LOG("Remove row: ", index, ", value: ", row.value(0).toInt());
    
    if(!row.isNull(0))
    {
        mUsedIds.removeAt(index);
        //commitData();
    }
}

void DataBaseTableDataModel::createTable()
{
    std::string statement = "CREATE TABLE " + mTableSchema->name() + " (";

    statement += "id INTEGER NOT NULL PRIMARY KEY";

    auto fields = mTableSchema->fields();

    for(int i = 0; i < static_cast<int>(fields.size()); ++i)
        statement += ", " + fields[i].mName + " " + mSQLTypes[static_cast<unsigned int>(fields[i].mType)];

    statement += ");";
    
    LOG("Create table statement: ", statement); 

    mTableSchema->clearChangeCache();

    database().transaction();
    QSqlQuery query(QString::fromStdString(statement), database());
    query.exec();
    database().commit();
    
    emit tableSchemaModified();
}

void DataBaseTableDataModel::alterTable()
{
    std::vector<std::string> statements;
    
    if(mTableSchema->fieldsToBeAdded().size())
    {
        for(const auto& pair : mTableSchema->fieldsToBeAdded())
        {
            std::string statement = "ALTER TABLE " + mTableSchema->name() + " ADD " +
            pair.second.mName + " " + mSQLTypes[static_cast<unsigned int>(pair.second.mType)] + ";";
            
            statements.push_back(statement);
        }
    }
    
    if(mTableSchema->fieldsToBeRenamed().size() || mTableSchema->fieldsToBeDropped().size() || mTableSchema->fieldsToBeTypeChanged().size())
    {
        const auto& renamedFields = mTableSchema->fieldsToBeRenamed();
        
        std::string alterStatement = "ALTER TABLE " + mTableSchema->name() + " RENAME TO " + mTableSchema->name() + "_old;";        
        
        std::string createStatement = "CREATE TABLE " + mTableSchema->name() + " (";    
        createStatement += "id INTEGER NOT NULL PRIMARY KEY, "; 
        
        auto fields = mTableSchema->fields();
        std::string fieldNames;
        std::string oldfieldNames;
        for(int i = 0; i < static_cast<int>(fields.size() - 1); ++i)
        {
            createStatement += fields[i].mName + " " + mSQLTypes[static_cast<unsigned int>(fields[i].mType)] + ", ";
            fieldNames += fields[i].mName + ", ";
            
            if(renamedFields.count(i) > 0)
                oldfieldNames += renamedFields.at(i).mName + ", ";
            else
                oldfieldNames += fields[i].mName + ", ";
        }
        
        createStatement += fields.back().mName + " " + mSQLTypes[static_cast<unsigned int>(fields.back().mType)];        
        createStatement += ");";
        fieldNames += fields.back().mName;

        if(renamedFields.count(fields.size() - 1) > 0)
            oldfieldNames += renamedFields.at(fields.size() - 1).mName;
        else
            oldfieldNames += fields.back().mName;        
        
        std::string copyStatement =   "INSERT INTO " + mTableSchema->name() + " (id, " + fieldNames + ") " +
                        "SELECT " + "id, " + oldfieldNames + " FROM " + mTableSchema->name() + "_old;";
        
        std::string dropStatement = "DROP TABLE " + mTableSchema->name() + "_old;";
        
        statements.push_back(alterStatement);
        statements.push_back(createStatement);
        statements.push_back(copyStatement);
        statements.push_back(dropStatement);
    }
    
    mTableSchema->clearChangeCache();
    
    database().transaction();
    for(auto& statement : statements)
    {   
        QSqlQuery query(database());
        query.exec(QString::fromStdString(statement));
    }
    database().commit(); 
    
    emit tableSchemaModified();
}
