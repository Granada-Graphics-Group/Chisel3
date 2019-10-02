#include "palettemodel.h"
#include "palette.h"

#include <QBrush>
#include <cmath>

PaletteModel::PaletteModel(Palette* palette)
:
    mPalette(palette)
{
    //mPalette = new Palette(0,  "Paleta", "", Palette::Type::Discrete, {{0.0, glm::vec4(1.0, 0.0, 0.0, 1.0)},{2.0, glm::vec4(1.0, 0.0, 1.0, 1.0)}});
}

PaletteModel::~PaletteModel()
{

}


// *** Public Methods *** //

//PaletteModel& PaletteModel::operator=(const PaletteModel& other)
//{
//
//}

double PaletteModel::value(int index) const
{
    return mPalette->controlPoint(index).first;
}

QModelIndex PaletteModel::indexOfValue(double value) const
{
    return index(mPalette->indexOfValue(value), 0);
}


double PaletteModel::lastUpdatedValueRow() const
{
    return std::distance(begin(mPalette->controlPoints()), mPalette->controlPoints().find(mLastUpdatedValue));
}

bool PaletteModel::containInf() const
{
    return mPalette->containControlPoint(std::numeric_limits<double>().infinity());
}

QVariant PaletteModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role != Qt::DisplayRole && role != Qt::BackgroundRole)
        return QVariant();
    
    if(mPalette != nullptr)
    {
        auto iter = mPalette->controlPoints().begin();
        
        std::advance(iter, index.row());
            
        if(index.column() == 0 && role == Qt::DisplayRole)
        {
            auto value = iter->first;
            
            if(fabs(round(value) - value) <= 0.00001f)
                return value;//QString::number(value);
            else
                return value;//QString::number(value, 'f', 6);
        }
        else if (index.column() == 1 && role == Qt::BackgroundRole)
        {
            auto color = iter->second;
            return QBrush(QColor::fromRgbF(color.x, color.y, color.z, color.w));
        }
    }
    return QVariant();
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex& index) const
{
    if (index.column() == 0)
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | QAbstractTableModel::flags(index);
    else
        return Qt::ItemIsEnabled;// | QAbstractTableModel::flags(index);
    
}


QVariant PaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) 
        {
            switch (section)
            {
            case 0:
                return QString("Value");
            case 1:
                return QString("Color");
            }
        }
    }
    
    return QVariant();        
}


int PaletteModel::columnCount(const QModelIndex& parent) const
{
    return 2;
}

int PaletteModel::rowCount(const QModelIndex& parent) const
{
    if(mPalette != nullptr)
        return mPalette->controlPoints().size();
    else
        return 0;
}

// QModelIndex PaletteModel::parent(const QModelIndex& child) const
// {
// 
// }

bool PaletteModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::EditRole)
    {
        if(index.column() == 0)
        {
            auto ctrlValue = data(index, Qt::DisplayRole).toDouble();
            auto newValue = value.toDouble();
            auto globalUpdate = newValue > mPalette->minValue() || newValue < mPalette->maxValue();
            
            mPalette->replaceControlPoint(ctrlValue, value.toDouble());
            
            mLastUpdatedValue = newValue;

            if(!globalUpdate)
                emit dataChanged(index, index, {Qt::EditRole});
            else
                emit dataChanged(this->index(0, 0), this->index(index.row(), 1), {Qt::EditRole});
            
        }
        else
        {
            auto newColor = value.value<QColor>();
            auto ctrlValue = data(this->index(index.row(), 0), Qt::DisplayRole).toDouble();
            mPalette->updateColor(ctrlValue, {newColor.redF(), newColor.greenF(), newColor.blueF(), newColor.alphaF()});
            emit dataChanged(index, index, {Qt::EditRole});
        }
        
    }
    return true;
}


void PaletteModel::setPalette(Palette* palette)
{   
    beginResetModel();
    mPalette = palette;    
    endResetModel();
}

void PaletteModel::addControlPoint()
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mPalette->addControlPoint(std::numeric_limits<double>().infinity(), {1.0, 1.0, 1.0, 1.0});
    endInsertRows();

    mAddControlPointState = true;
}

void PaletteModel::deleteControlPoint(const QModelIndex& controlPointIndex)
{
    auto valueIndex = index(controlPointIndex.row(), 0);
    auto value = data(valueIndex, Qt::DisplayRole).toDouble();
    beginRemoveRows(QModelIndex(), valueIndex.row(), valueIndex.row());
    mPalette->delControlPoint(value);
    endRemoveRows();
    
    emit dataChanged(valueIndex, this->index(rowCount() - 1, columnCount() - 1));
}

void PaletteModel::deleteInf()
{
    if(mPalette->containControlPoint(std::numeric_limits<double>().infinity()))
    {
        beginRemoveRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
        mPalette->delControlPoint(std::numeric_limits<double>().infinity());
        endRemoveRows();
    }
}

void PaletteModel::toggleIntepolation(bool value)
{
    mPalette->setInterpolation(value);
    
    emit interpolationChanged();
}


