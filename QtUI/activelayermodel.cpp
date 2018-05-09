#include "activelayermodel.h"
#include "disklayermodel.h"
#include "logger.hpp"

#include <QMimeData>
#include <QBrush>

ActiveLayerModel::ActiveLayerModel(DiskLayerModel* diskLayerModel, QObject* parent)
:
    QAbstractItemModel(parent),
    mDiskLayerModel(diskLayerModel)
{
    mRootItem = std::make_unique<ActiveLayerItem>(QList<QVariant>{"", "", "", ""});
    
//     setupModelData(data.split(QString("\n")), mRootItem.get());    
//     QStringList values {QString::number(18.99, 'f', 6), QString::number(20.33421, 'f', 6)};
//     QStringList values2 {QString::number(2.1, 'f', 6)};
//     QColor color(255, 125, 0);
//     QColor color2(0, 125, 255);
//     QColor color3(125, 255, 0);
//     addLayer("capa", {{color, values}, {color2, values2}}); 
//     
//     addLayerValues("capa", {{color3, values2}});
//     addLayer("capa2", {});
}

ActiveLayerModel::~ActiveLayerModel()
{
}

// *** Public Methods *** //
QVariant ActiveLayerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::ToolTipRole)
        return QVariant();

    ActiveLayerItem *item = static_cast<ActiveLayerItem*>(index.internalPointer());    
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return item->data(index.column());        
    }
    else if(role == Qt::ToolTipRole)
    {
        if(index.column() == 1 && item->data(index.column()) == true)
            return QString("Toggle database editor widget visibility");
        else if(index.column() == 2)
            return QString("Toggle layer visibility");
        else if(index.column() == 3)
            return QString("Change layer opacity");
    }
    
    return QVariant();
//     if (!index.isValid())
//         return QVariant();
//     
//     if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::BackgroundRole)
//         return QVariant();
// 
//     auto column = index.column();
//     auto item = static_cast<ActiveLayerItem*>(index.internalPointer());  
//     auto data = item->data(index.column());
//     auto temp = data.toString();
//     return data;
//     
//     if(column == 0)
//     {
//         if(item->parentItem() != mRootItem.get() && role == Qt::BackgroundRole)
//             return QBrush(qvariant_cast<QColor>(data));
//         else if(item->parentItem() == mRootItem.get() && (role == Qt::DisplayRole || role == Qt::EditRole))
//             return data;
//     }
//     else if(column != 0 && role == Qt::DisplayRole)
//         return data;
// 
//     return QVariant();
    
//     if (role == Qt::BackgroundRole && index.column() == 0)
//     {
//         //QBrush brush = qvariant_cast<QBrush>(item->data(index.column()));
//         return QBrush(qvariant_cast<QColor>(item->data(index.column())));
//     }
//     else
//     {
//         if (role != Qt::DisplayRole)
//             return QVariant();
// 
//         return item->data(index.column());
//     }
}

Qt::ItemFlags ActiveLayerModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
    else
    {
        if(!index.parent().isValid())
        {
            if(index.column() == 0)
                return Qt::ItemIsEditable | Qt::ItemIsDragEnabled | QAbstractItemModel::flags(index);
            else
                return Qt::ItemIsDragEnabled | QAbstractItemModel::flags(index);
        }
        else
            return QAbstractItemModel::flags(index);
    }
}

QVariant ActiveLayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return mRootItem->data(section);

    return QVariant();
}

QModelIndex ActiveLayerModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ActiveLayerItem *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem.get();
    else
        parentItem = static_cast<ActiveLayerItem*>(parent.internalPointer());

    ActiveLayerItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ActiveLayerModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    ActiveLayerItem *childItem = static_cast<ActiveLayerItem*>(index.internalPointer());
    ActiveLayerItem *parentItem = childItem->parentItem();

    if (parentItem == mRootItem.get())
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ActiveLayerModel::rowCount(const QModelIndex& parent) const
{
    ActiveLayerItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = mRootItem.get();
    else
        parentItem = static_cast<ActiveLayerItem*>(parent.internalPointer());

    return parentItem->childCount();
}


int ActiveLayerModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<ActiveLayerItem*>(parent.internalPointer())->columnCount();
    else
        return mRootItem->columnCount();
}

Qt::DropActions ActiveLayerModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList ActiveLayerModel::mimeTypes() const
{
return QAbstractItemModel::mimeTypes();
}

QMimeData* ActiveLayerModel::mimeData(const QModelIndexList& indexes) const
{
return QAbstractItemModel::mimeData(indexes);
}

bool ActiveLayerModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
return QAbstractItemModel::canDropMimeData(data, action, row, column, parent);
}

bool ActiveLayerModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;
    
    int beginRow;
    
    if(row != -1)
        beginRow = row;
    else if(parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());
     
    QByteArray encodedData = data->data("application/x-qabstractitemmodeldatalist");    
    QDataStream stream(&encodedData, QIODevice::ReadOnly);    
    QList<QMap<int, QVariant>> decodedData;
    int decodedRow;
    
    while(!stream.atEnd())
    {
        int decodedColumn;
        QMap<int, QVariant> decodedRoleData;
        stream >> decodedRow;
        stream >> decodedColumn;
        stream >> decodedRoleData;
        decodedData << decodedRoleData;
    }
        
//     auto name = decodedData.at(0)[Qt::DisplayRole].toString();
//     auto isRegisterType = decodedData.at(1)[Qt::DisplayRole].toBool();
//     auto visibility = decodedData.at(2)[Qt::DisplayRole].toBool();
//     auto opacity = decodedData.at(3)[Qt::DisplayRole].toBool();
// 
//     auto layer = new ActiveLayerItem({name, isRegisterType, visibility, opacity}, mRootItem.get());

    if (beginRow < decodedRow || beginRow > (decodedRow + 1) )
    {
        beginMoveRows(QModelIndex(), decodedRow, decodedRow, QModelIndex(), beginRow);
        LOG("Layer DecodedRow: ", decodedRow, " beginRow: ", beginRow);
        auto layer = mRootItem->child(decodedRow);
        LOG("Layer source: ", layer->data(0).toString().toStdString(), " from: ", decodedRow ," to: ", beginRow);
        mRootItem->insertChild(layer, beginRow);
        for(int i = 0; i < mRootItem->childCount(); ++i)
            LOG("Layer children: ", mRootItem->child(i)->data(0).toString().toStdString(), " pos: ", i);        
        if (decodedRow > beginRow)
        {            
            LOG("Layer remove: ", mRootItem->child(decodedRow + 1)->data(0).toString().toStdString(), " pos: ", decodedRow + 1);
            mRootItem->removeChild(decodedRow + 1);
        }
        else
        {
            LOG("Layer remove: ", mRootItem->child(decodedRow)->data(0).toString().toStdString(), " pos: ", decodedRow);
            mRootItem->removeChild(decodedRow);
        }
        endMoveRows();

        emit layerPositionChanged(decodedRow, beginRow);

        return true;
    }

    return false;
}

bool ActiveLayerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        ActiveLayerItem *item = static_cast<ActiveLayerItem*>(index.internalPointer());

        if (index.column() == 0)
        {
            if (value.toString().length() > 0)
            {
                auto filenames = mDiskLayerModel->filenames();
                auto found = std::find_if(std::begin(filenames), std::end(filenames), [&](const std::string& filename){ return (filename == value.toString().toStdString()) ? true : false;});
                
                if(found == std::end(filenames))
                {
                    item->setData(index.column(), value);                            

                    emit layerRenamed(value.toString());
                    emit dataChanged(index, index);

                    return true;
                }
            }
        }
        else
        {
            item->setData(index.column(), value);
                    
            emit dataChanged(index, index);

            return true;
        }
    }

    return false;
}

bool ActiveLayerModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);

    for(int i = row; i < row + count; ++i)
        mRootItem->insertChild(new ActiveLayerItem({"", false, true, false }, mRootItem.get()), row);

    endInsertRows();

    return true;
}

bool ActiveLayerModel::removeRows(int row, int count, const QModelIndex& parent)
{
    //int endRow = row + count - 1;
    //
    //beginRemoveRows(QModelIndex(), row, endRow);
    //
    //if(!parent.isValid())
    //{
    //    for(int i = row; i < row + count; ++i)
    //        mRootItem->removeChild(i);
    //}
    //else
    //    for(int i = row; i < row + count; ++i)
    //        static_cast<ActiveLayerItem*>(parent.internalPointer())->removeChild(i);
    //
    //endRemoveRows();
    //
    //return true;
    return false;
}


void ActiveLayerModel::addLayer(const QString name, bool isRegisterType, std::pair<int, int> resolution, const std::string type)
{
    auto layers = mRootItem->childs();
    bool found = false;
        
    for(auto &layer: layers)
        if(layer->data(0) == name)
            found = true;

    if(!found)
    {        
        auto layer = new ActiveLayerItem({name, isRegisterType, true, false}, mRootItem.get());
        auto childType = new ActiveLayerItem({QString::fromStdString("Type: " + type)}, layer);        
        auto childWidth = new ActiveLayerItem({QString::fromStdString("Width:  " + std::to_string(resolution.first))}, layer);
        auto childHeight = new ActiveLayerItem({QString::fromStdString("Height: " + std::to_string(resolution.second))}, layer);
        
        auto row = (rowCount() > 0) ? rowCount() - 1 : 0;
        beginInsertRows(QModelIndex(), row, row);
        mRootItem->prependChild(layer);
        endInsertRows();

        beginInsertRows(index(row,0), 0, 1);
        layer->appendChild(childType);
        layer->appendChild(childWidth);
        layer->appendChild(childHeight);
        endInsertRows();

        
//         beginInsertRows(index(rowCount(), 0), 0, values.count());
//         for(auto value: values)
//         {
//             QList<QVariant> colorDataValues;
//             colorDataValues << value.first;            
//             for(auto &data: value.second)
//                 colorDataValues << data;
//             layer->prependChild(new ActiveLayerItem(colorDataValues, layer));
//         } 
//         endInsertRows();
    }
    emit dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, columnCount() - 1));
    emit dataChanged(index(0, 0, index(rowCount() - 1, 0)), index(0, 2, index(rowCount() - 1, 0)));
}

void ActiveLayerModel::insertLayer(unsigned int row, const QString name, bool isRegisterType, std::pair<int, int> resolution, const std::string type)
{
    if(row <= rowCount())
    {
        auto layers = mRootItem->childs();
        bool found = false;
            
        for(auto &layer: layers)
            if(layer->data(0) == name)
                found = true;

        if(!found)
        {        
            auto layer = new ActiveLayerItem({name, isRegisterType, true, false}, mRootItem.get());
            auto childType = new ActiveLayerItem({QString::fromStdString("Type: " + type)}, layer);        
            auto childWidth = new ActiveLayerItem({QString::fromStdString("Width:  " + std::to_string(resolution.first))}, layer);
            auto childHeight = new ActiveLayerItem({QString::fromStdString("Height: " + std::to_string(resolution.second))}, layer);          
            beginInsertRows(QModelIndex(), row, row);
            mRootItem->insertChild(layer, row);
            endInsertRows();
            
            beginInsertRows(index(row,0), 0, 1);
            layer->appendChild(childType);
            layer->appendChild(childWidth);
            layer->appendChild(childHeight);
            endInsertRows();            
        }
        emit dataChanged(index(row, 0), index(rowCount() - 1, columnCount() - 1));
        emit dataChanged(index(0, 0, index(row, 0)), index(2, 0, index(row, 0)));
    }
}


void ActiveLayerModel::removeLayer(int row)
{
    if(row < rowCount())
    {
       beginRemoveRows(QModelIndex(), row, row); 
       mRootItem->removeChild(row);
       endRemoveRows();
       
       emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void ActiveLayerModel::removeLayers(int firstRow, int lastRow)
{
    if(firstRow >= 0 && lastRow >= 0 && lastRow < rowCount())
    {
        beginRemoveRows(QModelIndex(), firstRow, lastRow);
        for(auto i = lastRow; i >= firstRow; --i)
            mRootItem->removeChild(i);
        endRemoveRows();
       
       emit dataChanged(index(firstRow, 0), index(lastRow, columnCount() - 1));
    }    
}


void ActiveLayerModel::addLayerValues(const QString name, const QList< QPair< QColor, QStringList > >& values)
{
    auto layers = mRootItem->childs();
    auto found = mRootItem.get();
        
    for(auto &layer: layers)
        if(layer->data(0) == name)
           found = layer;
    
    if(found != mRootItem.get())
    {
        //TODO It's "working". Read Qt Documentation. Need improvement.
        beginInsertRows(QModelIndex(), rowCount(), values.count());
        //beginInsertRows(index(found->row(), 0), rowCount(), values.count());            
        for(auto& value: values)
        {
            QList<QVariant> colorDataValues;
            colorDataValues << value.first;
            for(auto &data: value.second)
                colorDataValues << data;
            found->appendChild(new ActiveLayerItem(colorDataValues, found));
        }
        endInsertRows();
        
        emit dataChanged(index(found->row(), 0), index(rowCount(), columnCount()));
    }
}


void ActiveLayerModel::addLayerValues(const QString& name, const QList< std::tuple< int, QColor, QStringList > >& values)
{
    auto layers = mRootItem->childs();
    auto found = mRootItem.get();
        
    for(auto &layer: layers)
        if(layer->data(0) == name)
           found = layer;
    
    if(found != mRootItem.get())
    {
        //TODO It's "working". Read Qt Documentation. Need improvement.
        //beginInsertRows(QModelIndex(), found->row(), found->row() + 1);
        beginInsertRows(QModelIndex(), rowCount(), values.count());
        //beginInsertRows(index(found->row(), 0), rowCount(), values.count());            
        for(auto& value: values)
        {
            QList<QVariant> colorDataValues;
            colorDataValues << std::get<1>(value);
            for(auto &data: std::get<2>(value))
                colorDataValues << data;
            found->insertChild(new ActiveLayerItem(colorDataValues, found), std::get<0>(value));
        }
        endInsertRows();
        
        //emit dataChanged(index(found->row(), 0), index(found->row(), columnCount()));
        emit dataChanged(index(found->row(), 0), index(rowCount(), columnCount()));
    }
}


bool ActiveLayerModel::updateLayerValues(const QString& name, const QList< std::tuple< int, int, QColor, QStringList > >& values)
{
    auto layers = mRootItem->childs();
    auto found = mRootItem.get();
        
    for(auto &layer: layers)
        if(layer->data(0) == name)
           found = layer;
    
    if(found != mRootItem.get())
    {
        //TODO It's "working". Read Qt Documentation. Need improvement.
        beginInsertRows(QModelIndex(), rowCount(), values.count());
        //beginInsertRows(index(found->row(), 0), rowCount(), values.count());            
        for(auto& value: values)
        {
            QList<QVariant> colorDataValues;
            colorDataValues << std::get<2>(value);
            for(auto &data: std::get<3>(value))
                colorDataValues << data;
            found->updateChild(std::get<0>(value), std::get<1>(value), colorDataValues);
        }
        endInsertRows();
        
        emit dataChanged(index(found->row(), 0), index(rowCount(), columnCount()));

        return true;
    }

    return false;
}


bool ActiveLayerModel::removeLayerValues(const QString& name, const QList< int >& values)
{
    auto layers = mRootItem->childs();
    auto found = mRootItem.get();
        
    for(auto &layer: layers)
        if(layer->data(0) == name)
           found = layer;
    
    if(found != mRootItem.get())
    {            
        for(auto& value: values)
        {
            removeRow(value, index(found->row(), 0));
        }
        
        emit dataChanged(index(found->row(), 0), index(rowCount(), columnCount()));

        return true;
    }
    return false;
}

void ActiveLayerModel::setVisibililty(int row, bool visibility)
{
    mRootItem->childs().at(row)->setData(2, visibility);
    emit dataChanged(index(row, 0), index(row, columnCount()));
}


// *** Private Methods *** //

void ActiveLayerModel::setupModelData(const QStringList& lines, ActiveLayerItem* parent)
{
    QList<ActiveLayerItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) 
        {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) 
            {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) 
                {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new ActiveLayerItem(columnData, parents.last()));
        }
        else
        {
            QList<QVariant> columColor;
            columColor << QBrush(Qt::red);
            columColor << "Rojo";
            parents.last()->appendChild(new ActiveLayerItem(columColor, parents.last()));
        }

        ++number;
    }
}

