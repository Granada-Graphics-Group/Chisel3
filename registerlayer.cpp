#include "registerlayer.h"

#include <algorithm>

RegisterLayer::RegisterLayer(std::string name, const std::pair<int, int>& resolution, const std::vector<std::pair<Texture *, Texture *> >& textures, Palette* palette)
:
    Layer(name, Layer::Type::Register, resolution, textures, palette)    
{
    mTableSchema = std::make_unique<DataBaseTable>(name);
    mSecondaryColumns.push_back(false);
}


// *** Public Methods *** //

const std::vector<DataBaseField> & RegisterLayer::fields() const
{
    return mTableSchema->fields();
}

const DataBaseField & RegisterLayer::field(std::string fieldName) const
{
    return mTableSchema->field(fieldName);
}

const DataBaseField & RegisterLayer::field(unsigned int index) const
{
    return mTableSchema->field(index);
}

std::string RegisterLayer::fieldName(unsigned int index) const
{
    return (index < mTableSchema->fields().size()) ? mTableSchema->field(index).mName : "";
}

bool RegisterLayer::fieldVisibility(unsigned int index) const
{
    return (index < mFieldsVisibility.size()) ? mFieldsVisibility[index] : false;
}

bool RegisterLayer::containField(std::string fieldName) const
{
    return mTableSchema->containField(fieldName);
}

bool RegisterLayer::containFieldType(DataBaseField::Type type) const
{
    return mTableSchema->containFieldType(type);
}

const std::map<int, DataBaseField> & RegisterLayer::fieldsToBeAdded() const
{
    return mTableSchema->fieldsToBeAdded();
}

const std::map<int, DataBaseField> & RegisterLayer::fieldsToBeDropped() const
{
    return mTableSchema->fieldsToBeDropped();
}

const std::map<int, DataBaseField> & RegisterLayer::fieldsToBeRenamed() const
{
    return mTableSchema->fieldsToBeRenamed();
}

const std::map<int, DataBaseField> & RegisterLayer::fieldsToBeTypeChanged() const
{
    return mTableSchema->fieldsToBeTypeChanged();
}

bool RegisterLayer::hasResources() const
{
    bool found = false;
    for(int i = 0; i < mTableSchema->fields().size() && !found; ++i)
        if(mTableSchema->field(i).mType == DataBaseField::Type::Resource)
            found = true;

    return found;
}

bool RegisterLayer::addField(const DataBaseField& field)
{
    auto result = mTableSchema->addField(field);
    
    if(result)
        mFieldsVisibility.push_back(true);
    
    return result;
}

bool RegisterLayer::removeField(unsigned int index)
{
    auto result = mTableSchema->removeField(index);
    
    if(result)
        mFieldsVisibility.erase(begin(mFieldsVisibility) + index);
    
    return result;
}

bool RegisterLayer::removeField(std::string name)
{
    return true;
}

bool RegisterLayer::changeFieldName(unsigned int index, std::string name)
{
    return mTableSchema->changeFieldName(index, name);
}

bool RegisterLayer::changeFieldType(unsigned int index, DataBaseField::Type type)
{
    return mTableSchema->changeFieldType(index, type);
}

bool RegisterLayer::changeFieldSize(unsigned int index, std::size_t size)
{
    return mTableSchema->changeFieldSize(index, size);
}

void RegisterLayer::copySchema(const DataBaseTable& schema)
{
    *mTableSchema = schema;
    mSecondaryColumns.resize(mTableSchema->fields().size() + 1, false);
    mFieldsVisibility.resize(mTableSchema->fields().size(), true);
    setDirty(true);
}

void RegisterLayer::copySchemaAndVisibility(const DataBaseTable& schema, const std::vector<bool>& visibility)
{
    *mTableSchema = schema;
    mFieldsVisibility = visibility;
    
    mSecondaryColumns.resize(mTableSchema->fields().size() + 1, false);
    
    setDirty(true);
}


void RegisterLayer::setPrimaryColumn(int primary)
{
    if(primary != mPrimaryColumn)
    {
        mPrimaryColumn = primary;
        setDirty(true);
    }
}

void RegisterLayer::setSecondaryColumns(const std::vector<bool>& secondaries)
{
    auto equal = std::equal(begin(mSecondaryColumns), end(mSecondaryColumns), begin(secondaries));

    if(!equal)
    {
        mSecondaryColumns = secondaries; 
        setDirty(true);
    }
}

void RegisterLayer::resizeFieldVisibility()
{
    if(mTableSchema->fields().size() != mFieldsVisibility.size())
        mFieldsVisibility.resize(mTableSchema->fields().size(), true);
}


void RegisterLayer::setFieldVisibility(unsigned int index, bool visible)
{
    if(index < mFieldsVisibility.size())
        mFieldsVisibility[index] = visible;
}

void RegisterLayer::setFieldsVisibility(const std::vector<bool>& visibility)
{
    if(visibility.size() == mTableSchema->fields().size())
        mFieldsVisibility = visibility;
}

