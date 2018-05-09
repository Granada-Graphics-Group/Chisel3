#include "tableproxyfilter.h"

TableProxyFilter::TableProxyFilter(QAbstractItemModel* sourceModel, QObject* parent)
:
    QSortFilterProxyModel(parent)
{
    setSourceModel(sourceModel);
}


// *** Public Methods *** //

Qt::ItemFlags TableProxyFilter::flags(const QModelIndex& index) const
{
    auto search = mColumnData.find(index.column());
    return (search != end(mColumnData)) ? search->second.second : QSortFilterProxyModel::flags(index);
}

QVariant TableProxyFilter::headerData(int section, Qt::Orientation orientation, int role) const
{
    return (mUseHorizontalHeader) ? QSortFilterProxyModel::headerData(section, orientation, role) : QVariant();        
}

void TableProxyFilter::setColumnEditable(unsigned int index, bool editable)
{
    auto flags = sourceModel()->flags(sourceModel()->index(0, index)) | Qt::ItemIsEnabled;    
    flags = (editable) ? flags | Qt::ItemIsEditable : flags & ~Qt::ItemIsEditable;

    auto search = mColumnData.find(index);        
    if(search != end(mColumnData))
        search->second.second = flags;    
    else
        mColumnData[index] = {true, flags};
}

void TableProxyFilter::setVisibleRows(const std::vector<int>& rows)
{
    mVisibleRows.clear();
    std::copy(begin(rows), end(rows), std::inserter(mVisibleRows, end(mVisibleRows)));
    invalidate();
}

void TableProxyFilter::setVisibleColumns(const std::vector<int>& columns)
{
    for(auto column : columns)
    {
        auto search = mColumnData.find(column);
    
        if(search != end(mColumnData))
            search->second.first = true;
        else
            mColumnData[column] = {true, sourceModel()->flags(sourceModel()->index(0, column)) | Qt::ItemIsEnabled};
    }
    invalidate();
}


// *** Protected Functions *** //

bool TableProxyFilter::filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const
{
    auto search = mColumnData.find(sourceColumn);
    return search != end(mColumnData) && search->second.first;
}

bool TableProxyFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    auto index = sourceModel()->index(sourceRow, 0, sourceParent);
    return mVisibleRows.find(sourceModel()->data(index).toInt()) != end(mVisibleRows);
}
