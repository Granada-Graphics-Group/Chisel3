#include "visiblecolumnmodel.h"
#include "databasetable.h"

#include <QColor>

VisibleColumnModel::VisibleColumnModel(int rows, int columns, QObject* parent)
:
    QStandardItemModel(rows, columns, parent)
{
    mSecondaryColumns.resize(columns, false);
}

// *** Public Methods *** //

QVariant VisibleColumnModel::data(const QModelIndex& index, int role) const
{
    if(index.isValid() && role == Qt::BackgroundRole)        
    {
        if(mPrimaryColumn > -1 && mPrimaryColumn == index.column())
            return QColor(255, 0, 0);
/*        else if(mSecondaryColumns[index.column()])
            return QColor(0, 0, 255);*/                        
    }
    
    return QStandardItemModel::data(index, role);    
}

void VisibleColumnModel::setColumnCount(int columns)
{
    auto oldsize = static_cast<int>(mSecondaryColumns.size());    
    auto difference = columns - oldsize;
    
    mSecondaryColumns.resize(columns);
    
    if(difference > 0)
        for(int i = oldsize; i < mSecondaryColumns.size(); ++i)
            mSecondaryColumns[i] = false;
    
    QStandardItemModel::setColumnCount(columns);    
}

void VisibleColumnModel::setColumns(int primary, const std::vector<bool>& secondaries)
{
    if(primary != 0 && (mTableSchema->fields().size() + 1) > primary && mTableSchema->field(primary - 1).mType < DataBaseField::Type::String)    
        mPrimaryColumn = primary;
    else
        mPrimaryColumn = 0;
    
    if(mPrimaryColumn != 0)
        mSecondaryColumns.assign(mSecondaryColumns.size(), false);    
    else
        mSecondaryColumns = secondaries;
    
    emit dataChanged(index(0, 0), index(0, columnCount() - 1));    
}


void VisibleColumnModel::setPrimary(int column)
{
    if(column == 0 || (column > 0 && mTableSchema->field(column - 1).mType < DataBaseField::Type::String)) 
    {
        mPrimaryColumn = column;
        
        if(mPrimaryColumn != 0)
            mSecondaryColumns.assign(mSecondaryColumns.size(), false);
        
        emit dataChanged(index(0, 0), index(0, columnCount() - 1));
    }
}

void VisibleColumnModel::setSecondaryColumns(const std::vector<bool>& columns)
{
    mSecondaryColumns = columns;
    emit dataChanged(index(0, 0), index(0, columnCount() - 1));
}


void VisibleColumnModel::setSecondaryColumn(int column)
{
    if(mSecondaryColumns.size() > column)
        mSecondaryColumns[column] = !mSecondaryColumns[column];
    
    emit dataChanged(index(0, 0), index(0, columnCount() - 1));
}

void VisibleColumnModel::addSecondaryColumn(int column)
{
    if(mSecondaryColumns.size() > column && !mSecondaryColumns[column])
        mSecondaryColumns[column] = true;
    
    emit dataChanged(index(0, 0), index(0, columnCount() - 1));    
}

void VisibleColumnModel::removeSecondaryColumn(int column)
{
    if(mSecondaryColumns.size() > column && mSecondaryColumns[column])
        mSecondaryColumns[column] = false;
    
    emit dataChanged(index(0, 0), index(0, columnCount() - 1));    
}

void VisibleColumnModel::clearColumns()
{
    mPrimaryColumn = -1;
    mSecondaryColumns.assign(mSecondaryColumns.size(), false);
    
    emit dataChanged(index(0, 0), index(0, columnCount() - 1));    
}

