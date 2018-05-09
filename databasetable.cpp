#include "databasetable.h"

#include <algorithm>

DataBaseTable::DataBaseTable(std::string name, const std::vector<DataBaseField>& fields)
:
    mName(name),
    mFields(fields)
{
    mFields.clear();
}

DataBaseTable::~DataBaseTable()
{

}

// *** Public Methods *** //

const DataBaseField& DataBaseTable::field(std::string fieldName) const
{
    auto found = std::find_if(begin(mFields), end(mFields), [&](const DataBaseField& cField){ return cField.mName == fieldName;});
    
    return *found;
}

const DataBaseField& DataBaseTable::field(unsigned int index) const
{
    return  mFields[index];// (index < static_cast<unsigned int>(mFields.size())) ? mFields[index] : nullptr;
}

bool DataBaseTable::containField(std::string fieldName) const
{
    auto found = std::find_if(begin(mFields), end(mFields), [&](const DataBaseField& cField){ return cField.mName == fieldName;});    
    
    return (found != end(mFields)) ? true : false;
}

bool DataBaseTable::containFieldType(DataBaseField::Type type) const
{
    auto found = std::find_if(begin(mFields), end(mFields), [&](const DataBaseField& cField){ return cField.mType == type;});

    return (found != end(mFields)) ? true : false;
}


bool DataBaseTable::addField(const DataBaseField& field)
{
    auto found = std::find_if(begin(mFields), end(mFields), [&field](const DataBaseField& cField){ return cField.mName == field.mName;});
    
    if(found == end(mFields))
    {
        mFieldsToAdd[mFields.size()] = field;
        mFields.push_back(field);
        mDirtyFlag = true;
        
        return true;
    }

    return false;
}

bool DataBaseTable::removeField(std::string name)
{
    auto found = std::find_if(begin(mFields), end(mFields), [&name](const DataBaseField& cField){ return cField.mName == name;});
    
    if(found != end(mFields))
    {
        if(mFieldsToAdd.count(found - begin(mFields)) > 0)
            mFieldsToAdd.erase(found - begin(mFields));
        else                    
            mFieldsToDrop[found - begin(mFields)] = *found;
        
        mFields.erase(found);
        mDirtyFlag = true;
        
        return true;
    }

    return false;
}


bool DataBaseTable::removeField(unsigned int index)
{
    if(index < static_cast<unsigned int>(mFields.size()))
    {
        if(mFieldsToAdd.count(index) > 0)
            mFieldsToAdd.erase(index);
        else            
            mFieldsToDrop[index] = mFields[index];
        
        mFields.erase(begin(mFields) + index);
        mDirtyFlag = true;
        
        return true;
    }
    
    return false;
}

void DataBaseTable::clear()
{
    clearChangeCache();    
    mFields.clear();
    mDirtyFlag = true;
}

bool DataBaseTable::changeFieldName(unsigned int index, std::string name)
{
    if(index < static_cast<unsigned int>(mFields.size()))
    {
        if(mFieldsToAdd.count(index) > 0)
            mFieldsToAdd[index].mName = name; 
        else
            mFieldsToRename[index] = mFields[index];
        
        mFields[index].mName = name;
        mDirtyFlag = true;
        
        return true;
    }
    
    return false;    
}

bool DataBaseTable::changeFieldType(unsigned int index, DataBaseField::Type type)
{
    if(index < static_cast<unsigned int>(mFields.size()))
    {
        mFields[index].mType = type;
        
        if(mFieldsToAdd.count(index) > 0)
            mFieldsToAdd[index].mType = type; 
        else
            mFieldsToChangeType[index] = mFields[index];        
                
        mDirtyFlag = true;
        
        return true;
    }
    
    return false;
}

bool DataBaseTable::changeFieldSize(unsigned int index, std::size_t size)
{
    if(index < static_cast<unsigned int>(mFields.size()))
    {
        mFields[index].mSize = size;
        mDirtyFlag = true;
        
        return true;
    }
    
    return false;
}

void DataBaseTable::clearChangeCache()
{
    mFieldsToAdd.clear();
    mFieldsToDrop.clear();
    mFieldsToRename.clear();
    mFieldsToChangeType.clear();
    mDirtyFlag = false;
}
