#include "disklayermodel.h"

DiskLayerModel::DiskLayerModel(std::string path, QObject* parent)
:
    QFileSystemModel(parent)
{
    setFilter(QDir::Files);
    setNameFilters({"*.lay"});
    setRootPath(QString::fromStdString(path));
    setReadOnly(false);
}


// *** Public Methods *** //

QVariant DiskLayerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !mEnable)
        return QModelIndex();
    
    if (index.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
        return (mEdit) ? QFileSystemModel::data(index, role) : fileInfo(index).baseName();
    else if(index.column() == 1)
    {
        auto fileName = fileInfo(index).baseName();
        auto search = mActiveFiles.find(fileName.toStdString());
        return (role == Qt::BackgroundRole || role == Qt::ForegroundRole) ? ((search != end(mActiveFiles) && search->second) ? QColor(0, 240, 0) : QVariant()) : QVariant();
    }
    else
        return QFileSystemModel::data(index, role);    
}

Qt::ItemFlags DiskLayerModel::flags(const QModelIndex& index) const
{
    auto flags = QFileSystemModel::flags(index);
    
    if(index.isValid() && index.column() == 1)
    {
        auto fileName = fileInfo(index).baseName();
        auto search = mActiveFiles.find(fileName.toStdString());
            
        if(search != end(mActiveFiles) && search->second)            
            flags = flags & ~Qt::ItemIsSelectable;
    }
    
    return flags;
}

// QVariant DiskLayerModel::headerData(int section, Qt::Orientation orientation, int role) const
// {
//     if(role == Qt::DecorationRole && section == 0)
//         return QVariant();
//     else
//         QFileSystemModel::headerData(section, orientation, role);
// }

std::vector<std::string> DiskLayerModel::filenames() const
{
    std::vector<std::string> files;
    
    auto rootIndex = index(rootPath());
    auto count = rowCount(rootIndex);
    
    for(int i = 0;  i < count; ++i)
    {
        auto currentIndex = index(i, 0, rootIndex);
        
        if(!isDir(currentIndex))
            files.push_back(data(currentIndex, Qt::DisplayRole).toString().toStdString());
    }
    
    return files;
}

unsigned int DiskLayerModel::layerCount() const
{
    unsigned int layerCount = 0;
    
    auto rootIndex = index(rootPath());
    auto elemCount = rowCount(rootIndex);
    
    for(int i = 0;  i < elemCount; ++i)
        if(!isDir(index(i, 0, rootIndex)))
            layerCount++;
        
    return layerCount;
}

bool DiskLayerModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.column() == 0 && role == Qt::EditRole)
    {
        mEdit = true;
        auto result = QFileSystemModel::setData(index, QVariant(value.toString() + ".lay"), role);
        mEdit = false;
        return result;
    }
    else
        return QFileSystemModel::setData(index, value, role);    
}

void DiskLayerModel::setNewRootPath(std::string path, bool clearFileState)
{
    setRootPath(QString::fromStdString(path));
    setReadOnly(false);
    
    if(clearFileState)
    {
//         mFileState.clear();
        mActiveFiles.clear();
    }
//     mDirectoryLoaded = false;
    mEnable = true;
}

void DiskLayerModel::setFileState(std::string name, bool loaded)
{
//     if(mDirectoryLoaded)
//     {
//         auto fileIndex = index(rootPath() + "/" + QString::fromStdString(name) + ".lay");
//         
//         if(fileIndex.isValid())
//         {   
//             if(mFileState.size() <= fileIndex.row())
//                 mFileState.resize(fileIndex.row() + 1);
//             
//             mFileState[fileIndex.row()] = loaded;
//             emit dataChanged(index(fileIndex.row(), 1), index(fileIndex.row(), 1));
//         }
//     }
//     else
//         mCachedFileState.push_back({name, loaded});
    
    mActiveFiles[name] = loaded;
}

void DiskLayerModel::renameActiveFile(std::string oldName, std::string newName)
{
    mActiveFiles[newName] = mActiveFiles[oldName];
    mActiveFiles.erase(oldName);
}


void DiskLayerModel::resizeFileState()
{
//     mDirectoryLoaded = true;
//     
//     mFileState.resize(rowCount(index(rootPath())));
//     
//     if(mCachedFileState.size())
//     {
//         for(const auto& pair: mCachedFileState)
//             setFileState(pair.first, pair.second);
//         
//         mCachedFileState.clear();
//     }   
}


