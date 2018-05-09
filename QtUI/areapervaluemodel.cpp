#include "areapervaluemodel.h"

AreaPerValueModel::AreaPerValueModel(QObject* parent)
:
    QAbstractTableModel(parent)
{
    
}

// *** Public Methods *** //

Qt::ItemFlags AreaPerValueModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant AreaPerValueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return (section == 0) ? QString("Value") : QString("Area");
    
    return QVariant();
}

QVariant AreaPerValueModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return mData[index.row()][index.column()];      
}

int AreaPerValueModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(mData.size());
}


int AreaPerValueModel::columnCount(const QModelIndex& parent) const
{
    return 2;
}

void AreaPerValueModel::setAreaPerValue(const std::vector<std::array<float, 2> >& areaPerValue, float unusedArea)
{
    for(const auto& valuePair: areaPerValue)
        mData.push_back({{valuePair[0], valuePair[1]}});
    
    mData.push_back({{"Unused", unusedArea}});
}
