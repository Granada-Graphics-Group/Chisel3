#ifndef DATABASERESOURCEMODEL_H
#define DATABASERESOURCEMODEL_H

#include <QFileSystemModel>

class DataBaseResourceModel : public QFileSystemModel
{
    Q_OBJECT
    
public:
    DataBaseResourceModel(std::string path, QObject *parent = Q_NULLPTR);

    std::vector<std::string> resourceFileNames(std::string dirName) const;
//     QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
//     Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
//     
//     bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    void setNewRootPath(std::string path);


private:

};

#endif
