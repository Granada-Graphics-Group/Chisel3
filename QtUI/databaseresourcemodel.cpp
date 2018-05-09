#include "databaseresourcemodel.h"

DataBaseResourceModel::DataBaseResourceModel(std::string path, QObject* parent)
:
    QFileSystemModel(parent)
{
    setRootPath(QString::fromStdString(path));
    setReadOnly(false);    
}


// *** Public Methods *** //

std::vector<std::string> DataBaseResourceModel::resourceFileNames(std::string dirName) const
{
    std::vector<std::string> files;

    auto dirIndex = index(rootPath() + "/" + QString::fromStdString(dirName));
    auto count = rowCount(dirIndex);

    for (int i = 0; i < count; ++i)
    {
        auto currentIndex = index(i, 0, dirIndex);

        if (!isDir(currentIndex))
            files.push_back(data(currentIndex, Qt::DisplayRole).toString().toStdString());
    }

    return files;
}

void DataBaseResourceModel::setNewRootPath(std::string path)
{
    setRootPath(QString::fromStdString(path));
    setReadOnly(false);    
}

