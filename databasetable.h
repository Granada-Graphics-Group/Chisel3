#ifndef DATABASETABLE_H
#define DATABASETABLE_H

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <vector>
#include <map>
#include <string>


struct DataBaseField
{
    enum class Type: unsigned int
    {
        Integer = 0,
        Double = 1,
        String = 2,
        Date = 3,        
        Resource = 4,
        Area = 5
    };
    
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(mName, mType, mSize);
    }
    
    DataBaseField(){};
    DataBaseField(std::string name, Type type, std::size_t size): mName(name), mType(type), mSize(size){};
    
    bool isInteger() const { return mType == Type::Integer; }
    bool isDouble() const { return mType == Type::Double; }
    bool isString() const { return mType == Type::String; }
    bool isDate() const { return mType == Type::Date; }
    bool isResource() const { return mType == Type::Resource; }
    bool isArea() const { return mType == Type::Area; }
    
    std::string mName;
    Type mType;
    std::size_t mSize;
};


class DataBaseTable
{
public:
    DataBaseTable() : DataBaseTable("") {};
    DataBaseTable(std::string name): DataBaseTable(name, {}){};
    DataBaseTable(std::string name, const std::vector<DataBaseField>& fields);
    
    std::string name() const { return mName; }
    const std::vector<DataBaseField>& fields() const { return mFields; }
    const DataBaseField& field(std::string fieldName) const;
    const DataBaseField& field(unsigned int index) const;
    bool containField(std::string fieldName) const;
    bool containFieldType(DataBaseField::Type type) const;
    bool isDirty() const { return mDirtyFlag; }
    const std::map<int, DataBaseField>& fieldsToBeAdded() const { return mFieldsToAdd; }
    const std::map<int, DataBaseField>& fieldsToBeDropped() const { return mFieldsToDrop; }
    const std::map<int, DataBaseField>& fieldsToBeRenamed() const { return mFieldsToRename; }
    const std::map<int, DataBaseField>& fieldsToBeTypeChanged() const { return mFieldsToChangeType; }
    
    void setName(std::string name){ mName = name;}
    bool addField(const DataBaseField& field);
    bool removeField(std::string name);
    bool removeField(unsigned int index);
    void clear();
    bool changeFieldName(unsigned int index, std::string name);
    bool changeFieldType(unsigned int index, DataBaseField::Type type);
    bool changeFieldSize(unsigned int index, std::size_t size);
    void clearChangeCache();
    void setDirty(bool value) { mDirtyFlag = value; }
    
    ~DataBaseTable();
    
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(mName, mFields);
    }     
    
private:
    std::string mName;
    std::vector<DataBaseField> mFields;
    std::map<int, DataBaseField> mFieldsToAdd;
    std::map<int, DataBaseField> mFieldsToDrop;
    std::map<int, DataBaseField> mFieldsToRename;
    std::map<int, DataBaseField> mFieldsToChangeType;

    bool mDirtyFlag = false;
};

#endif // DATABASETABLE_H

