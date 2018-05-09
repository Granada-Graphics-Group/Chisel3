#include "databasetableschemamodel.h"
#include "registerlayer.h"


DataBaseTableSchemaModel::DataBaseTableSchemaModel(QObject* parent)
:
    QAbstractTableModel(parent)
{

}

DataBaseTableSchemaModel::~DataBaseTableSchemaModel()
{

}

// QModelIndex DataBaseTableDefinitionModel::index(int row, int column, const QModelIndex& parent) const
// {
// 
// }

Qt::ItemFlags DataBaseTableSchemaModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant DataBaseTableSchemaModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical) 
        {
            switch (section)
            {
            case 0:
                return QString("Name");
            case 1:
                return QString("Type");
            }
        }
    }       
    
    return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant DataBaseTableSchemaModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role != Qt::DisplayRole && role != Qt::UserRole)
        return QVariant();
    
    auto field = mLayer->field(index.column());
    
    if(role == Qt::DisplayRole)
    {
        if(index.row() == 0)
        {
            return QString(field.mName.c_str());
        }
        else if(index.row() == 1)
            return mFieldTypes[static_cast<unsigned int>(field.mType)];
    }
    else if (role == Qt::UserRole && index.row() == 1)
        return static_cast<unsigned int>(field.mType);
    
    return QVariant();
}

int DataBaseTableSchemaModel::columnCount(const QModelIndex& parent) const
{
   return (mLayer != nullptr) ? static_cast<int>(mLayer->fieldCount()) : 0;
}

int DataBaseTableSchemaModel::rowCount(const QModelIndex& parent) const
{
    return 2;
}

// QModelIndex DataBaseTableDefinitionModel::parent(const QModelIndex& child) const
// {
// 
// }

DataBaseTable * DataBaseTableSchemaModel::schema() const
{
    return (mLayer != nullptr) ? mLayer->tableSchema() : nullptr;
}

bool DataBaseTableSchemaModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(role == Qt::EditRole)
    {
        if(index.row() == 0)
            mLayer->changeFieldName(index.column(), qvariant_cast<QString>(value).toStdString());
        else if(index.row() == 1)
            mLayer->changeFieldType(index.column(), static_cast<DataBaseField::Type>(value.toUInt()));
        
        emit dataChanged(index, index);

        return true;
    }

    return false;
}

// void DataBaseTableSchemaModel::setTableSchema(DataBaseTable* schema)
// {
//     //auto oldFieldCount = (mTableSchema != nullptr) ? columnCount() : 0;
//     
//     beginResetModel();
//     mTableSchema = schema;
//     endResetModel();
//     
// //     if (mTableSchema->fields().size() > 0)
// //     {
// //         beginInsertColumns(QModelIndex(), oldFieldCount - 1, columnCount() - 1);
// //         endInsertColumns();
// // 
// //         emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
// //     }
// }

void DataBaseTableSchemaModel::setDatabaseLayer(RegisterLayer* layer)
{
    beginResetModel();
    mLayer = layer;
    endResetModel();    
}


void DataBaseTableSchemaModel::copySchemaContents(const DataBaseTable& schema)
{
    beginResetModel();
    mLayer->copySchema(schema);
    endResetModel();    
}



// *** Public Slots *** //

void DataBaseTableSchemaModel::addField()
{
    auto added = mLayer->addField({"", DataBaseField::Type::Integer, 0});
    
    if(added)
    {
        beginInsertColumns(QModelIndex(), columnCount() - 1, columnCount() - 1);
        endInsertColumns();
    
        emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
    }
}

void DataBaseTableSchemaModel::deleteField()
{
    if(columnCount() > 1)
    {
        auto columns = columnCount();
        
        beginRemoveColumns(QModelIndex(), columns - 1, columns - 1);
        mLayer->removeField(columns - 1);
        endRemoveColumns();
        
        emit dataChanged(index(0, columns - 1), index(rowCount(), columns - 1));
    }
}

void DataBaseTableSchemaModel::deleteField(int fieldIndex)
{
    if(fieldIndex < columnCount())
    {
        beginRemoveColumns(QModelIndex(), fieldIndex, fieldIndex);
        mLayer->removeField(fieldIndex);
        endRemoveColumns();
        
        emit dataChanged(index(0, fieldIndex), index(rowCount(), columnCount() - 1));
    }    
}

void DataBaseTableSchemaModel::clearSchema()
{
    if(mLayer != nullptr)
    {
        beginResetModel();
        mLayer->tableSchema()->clear();
        endResetModel();
    }
}

void DataBaseTableSchemaModel::createTableSchemaStatement()
{
    if(mLayer->fieldCount() > 0)
    {
        std::string statement = "CREATE TABLE " + mLayer->name() + " (";
        
        statement += "id INTEGER NOT NULL PRIMARY KEY, "; 
        
        auto fields = mLayer->fields();
    
        for(int i = 0; i < static_cast<int>(fields.size() - 1); ++i)        
            statement += fields[i].mName + " " + mSQLTypes[static_cast<unsigned int>(fields[i].mType)].toStdString() + ", ";    
        
        statement += fields.back().mName + " " + mSQLTypes[static_cast<unsigned int>(fields.back().mType)].toStdString();    
        
        statement += ");";
    }
    
    //emit tableSchemaCreated(statement);
}
