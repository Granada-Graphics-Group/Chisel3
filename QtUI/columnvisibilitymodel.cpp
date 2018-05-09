#include "columnvisibilitymodel.h"
#include "registerlayer.h"

ColumnVisibilityModel::ColumnVisibilityModel(QObject* parent)
:
    QAbstractListModel(parent)
{
    
}


// *** Public Methods *** //

Qt::ItemFlags ColumnVisibilityModel::flags(const QModelIndex& index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    else
        return 0;
}

QVariant ColumnVisibilityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)     
        return QVariant("Columns");
    else
        return QVariant();
}

QVariant ColumnVisibilityModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role == Qt::DisplayRole) 
        return QVariant(QString::fromStdString(mLayer->fieldName(index.row())));
    else if(role == Qt::CheckStateRole)
        return mNewColumnVisibility[index.row()] ? Qt::Checked : Qt::Unchecked;
    
    return QVariant();
}

int ColumnVisibilityModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(mNewColumnVisibility.size());
}

const std::vector<bool> ColumnVisibilityModel::columnVisibility() const
{
    return mLayer->fieldsVisibility();
}

bool ColumnVisibilityModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;
    
    if(role == Qt::CheckStateRole)
    {
        mNewColumnVisibility[index.row()] = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked) ? true : false;
        mDirty = true;
        emit dataChanged(index, index);
        
        return true;
    }
    
    return false;
}

void ColumnVisibilityModel::setLayer(RegisterLayer* layer)
{
    beginResetModel();
    mLayer = layer;
    mNewColumnVisibility = layer->fieldsVisibility();
    endResetModel();    
}

void ColumnVisibilityModel::setColumnVisibility(const std::vector<bool>& columnVisibility)
{
    beginResetModel();
    mLayer->setFieldsVisibility(columnVisibility);
    mNewColumnVisibility = columnVisibility;
    endResetModel();        
}

void ColumnVisibilityModel::applyChanges()
{
    mLayer->setFieldsVisibility(mNewColumnVisibility);
    mDirty = false;
}

void ColumnVisibilityModel::updateVisibilityWithSourceLayer()
{
    beginResetModel();
    mNewColumnVisibility = mLayer->fieldsVisibility();
    mDirty = false;
    endResetModel();        
}


