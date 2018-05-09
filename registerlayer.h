#ifndef REGISTER_LAYER_H
#define REGISTER_LAYER_H

#include "layer.h"
#include "databasetable.h"

#include <memory>


class RegisterLayer: public Layer
{
public:
    RegisterLayer(std::string name, const std::pair< int, int >& resolution, const std::vector< std::pair< Texture*, Texture* > >& textures, Palette* palette = nullptr);
    
    bool isDirty() const { return Layer::isDirty() || mTableSchema->isDirty();  }
    bool isAreaFieldDirty() const { return mAreaFieldDirty; }
    DataBaseTable* tableSchema() const { return mTableSchema.get(); }

    const std::vector<DataBaseField>& fields() const;
    const std::vector<bool>& fieldsVisibility() const { return mFieldsVisibility; }
    int fieldCount() const { return mTableSchema->fields().size(); }
    const DataBaseField& field(std::string fieldName) const;
    const DataBaseField& field(unsigned int index) const;
    std::string fieldName(unsigned int index) const;
    bool fieldVisibility(unsigned int index) const;
    bool containField(std::string fieldName) const;
    bool containFieldType(DataBaseField::Type type) const;
    const std::map<int, DataBaseField>& fieldsToBeAdded() const;
    const std::map<int, DataBaseField>& fieldsToBeDropped() const;
    const std::map<int, DataBaseField>& fieldsToBeRenamed() const;
    const std::map<int, DataBaseField>& fieldsToBeTypeChanged() const;
    std::string tableFilter() const { return mTableFilter; }
    
    int primaryColumn() const { return mPrimaryColumn; }
    const std::vector<bool>& secondaryColumns() const { return mSecondaryColumns; }
    bool hasResources() const;
    
    bool addField(const DataBaseField& field);
    bool removeField(std::string name);
    bool removeField(unsigned int index);
    bool changeFieldName(unsigned int index, std::string name);
    bool changeFieldType(unsigned int index, DataBaseField::Type type);
    bool changeFieldSize(unsigned int index, std::size_t size);
    void copySchema(const DataBaseTable& schema);
    void copySchemaAndVisibility(const DataBaseTable& schema, const std::vector<bool>& visibility);
    void setTableFilter(const std::string& filter) { mTableFilter = filter; };
    
    void setPrimaryColumn(int primary);
    void setSecondaryColumns(const std::vector<bool>& secondaries);
    void clearSecondaryColumns() { mSecondaryColumns.assign(mSecondaryColumns.size(), false); };
    void setAreaFieldDirty(bool dirty) { mAreaFieldDirty = dirty; }
    void resizeFieldVisibility();
    void setFieldVisibility(unsigned int index, bool visible);
    void setFieldsVisibility(const std::vector<bool>& visibility);
      
private:
    std::unique_ptr<DataBaseTable> mTableSchema;
    int mPrimaryColumn = 0;
    std::vector<bool> mSecondaryColumns;
    std::vector<bool> mFieldsVisibility;    
    bool mAreaFieldDirty = false;
    std::string mTableFilter;
};

#endif
