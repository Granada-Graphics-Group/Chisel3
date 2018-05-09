#include "palettelistmodel.h"
#include "resourcemanager.h"
#include "palette.h"

#include <QPainter>
#include <QColor>
#include <QBrush>

PaletteListModel::PaletteListModel(ResourceManager* manager)
: 
    mResourceManager(manager)
{

}

PaletteListModel::~PaletteListModel()
{

}


// *** Public Methods *** //

QVariant PaletteListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::UserRole)
        return QVariant();
        
    if(role == Qt::DisplayRole)
    {
        QVariant paletteVariant;                
        auto pal = mResourceManager->palette(index.row());
        paletteVariant.setValue(mResourceManager->palette(index.row()));
        
        return paletteVariant;
    }
    else if(role == Qt::EditRole)
    {
        QVariant paletteName;
        paletteName.setValue(QString::fromStdString(mResourceManager->palette(index.row())->name()));
        
        return paletteName;
    }
    else
        return mCurrentLayerType;
//     QVariant nueva(palette);
//     QList < QPair < double, QColor> > list;
//     hola.setValue<QList < QPair < double, QColor> >>(list);
    
//     if(column == 0 && role == Qt::DisplayRole)
//         return QString::fromStdString(palette->name());
//     else if(column == 1 && role == Qt::BackgroundRole)
//     {
//         auto min = palette->minValue();
//         auto max = palette->maxValue();
//         auto viewRect = QRectF(QPoint(0, 0), QPoint(200, 10));
// 
//         if(palette->type() == Palette::Type::Discrete)
//         {
//             const auto& ctrlPoints = palette->controlPoints();
//             auto count = ctrlPoints.rbegin()->first - ctrlPoints.begin()->first + 1;
//             QImage discrete(100, 10, QImage::Format_ARGB32);
//             auto colorWidth = ceil(discrete.width() / count);
//             QPainter painter(&discrete);
//             for (auto i = 0; i < count; ++i)
//             {
//                 auto color = palette->color(ctrlPoints.begin()->first + i);
//                 painter.setBrush(QBrush(QColor::fromRgbF(color.r, color.g, color.b, color.a)));
//                 painter.setPen(Qt::NoPen);
//                 //painter.drawRect(i * colorWidth, discrete.height(), colorWidth, discrete.height());
//                 painter.drawRect(i * colorWidth, 0, colorWidth, discrete.height());
//                 //mScene->addRect(0, i * colorHeight, viewRect.width(), colorHeight, { Qt::NoPen }, QBrush(QColor::fromRgbF(color.r, color.g, color.b, color.a)));
//             }
//             painter.end();
//             
//             return QBrush(discrete);
//         }
//         else
//         {
//             QLinearGradient gradient(viewRect.topLeft(), viewRect.bottomRight());
//             
//             gradient.setSpread(QGradient::RepeatSpread);
//             
//             for(const auto& ctrlPoint: palette->controlPoints())
//             {
//                 auto color = ctrlPoint.second;
//                 auto value = ctrlPoint.first;            
//                 gradient.setColorAt((ctrlPoint.first - min) / (max - min), QColor::fromRgbF(color.r, color.g, color.b, color.a));
//             }
//             
//             return QBrush(gradient);
// 
//         }
//         
//     }
    
    return QVariant();
}

Qt::ItemFlags PaletteListModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return Qt::ItemIsSelectable | QAbstractListModel::flags(index);
    else
        return 0;
}

QVariant PaletteListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) 
        {
            switch (section)
            {
            case 0:
                return QString("Name");
            case 1:
                return QString("Preview");
            }
        }
    }
    
    return QVariant();    
}

// QModelIndex PaletteListModel::index(int row, int column, const QModelIndex &parent) const
// {
//     if (!hasIndex(row, column, parent))
//         return QModelIndex();
// 
//     if(row < static_cast<int>(mPalettes.size() + mLocalPalettes.size()))
//         return createIndex(row, column);
//     else
//         return QModelIndex();
// }

QModelIndex PaletteListModel::parent(const QModelIndex &index) const
{
//     if (!index.isValid())
//         return QModelIndex();

//     ActiveLayerItem *childItem = static_cast<ActiveLayerItem*>(index.internalPointer());
//     ActiveLayerItem *parentItem = childItem->parentItem();
// 
//     if (parentItem == mRootItem.get())
//         return QModelIndex();

    return QModelIndex();// createIndex(parentItem->row(), 0, parentItem);    
}

int PaletteListModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

int PaletteListModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(mPalettes.size() + mLocalPalettes.size());
}

bool PaletteListModel::setData(const QModelIndex & index, const QVariant & value, int role)
{    
    if(index.isValid())
    {
        mResourceManager->renamePaletteFile(index.row(), value.toString().toStdString());
        
        return true;
    }
    
    return false;
}

void PaletteListModel::populateList(const std::vector< Palette* >& list)
{    
    beginInsertRows(QModelIndex(), 0, list.size() - 1);
    mPalettes = list;
    endInsertRows();
    
    emit dataChanged(index(0, 0), index(mPalettes.size() - 1, 0));    
}

void PaletteListModel::populateLocalList(const std::vector<Palette *>& list)
{
    beginInsertRows(QModelIndex(), mPalettes.size(), mPalettes.size() + list.size() - 1);
    mLocalPalettes = list;
    endInsertRows();
    
    emit dataChanged(index(mPalettes.size(), 0), index(mPalettes.size() + list.size() - 1, 0));
}

void PaletteListModel::addPalette(Palette* palette)
{
    beginInsertRows(QModelIndex(), mPalettes.size(), mPalettes.size());
    mPalettes.push_back(palette);
    endInsertRows();
    
    emit dataChanged(index(mPalettes.size() - 1, 0), index(mPalettes.size() - 1, 0));    
}

void PaletteListModel::delPalette(int row)
{
    if(row < mPalettes.size())
    {
        beginRemoveRows(QModelIndex(), row, row);
        mPalettes.erase(begin(mPalettes) + row);
        endRemoveRows();
        emit dataChanged(index(row, 0), index(mPalettes.size() - 1, 0));    
    }
    
/*    
    beginResetModel();
    mPalettes.erase(begin(mPalettes) + row);
    endResetModel();*/
}


void PaletteListModel::deleteLocalPalette(int row)
{
    int localRow = row - static_cast<int>(mPalettes.size());
    if(localRow >= 0)
    {
        beginRemoveRows(QModelIndex(), mPalettes.size() + localRow, mPalettes.size() + localRow);
        mLocalPalettes.erase(begin(mLocalPalettes) + localRow);
        endRemoveRows();
        emit dataChanged(index(mPalettes.size() + localRow, 0), index(mPalettes.size() + localRow, 0));
    }
}


void PaletteListModel::addLocalPalette(Palette* palette)
{
    beginInsertRows(QModelIndex(), mPalettes.size() + mLocalPalettes.size() - 1, mPalettes.size() + mLocalPalettes.size() - 1);
    mLocalPalettes.push_back(palette);
    endInsertRows();
    
    emit dataChanged(index(mPalettes.size() + mLocalPalettes.size() - 1, 0), index(mPalettes.size() + mLocalPalettes.size() - 1, 0));    
}



void PaletteListModel::setCurrentLayerType(bool discrete)
{
    mCurrentLayerType = discrete;
    
    emit dataChanged(index(0, 0), index(mPalettes.size() + mLocalPalettes.size() - 1, 0));
}
