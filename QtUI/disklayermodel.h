#ifndef DISKLAYERMODEL_H
#define DISKLAYERMODEL_H

#include <QFileSystemModel>

class DiskLayerModel : public QFileSystemModel
{
    Q_OBJECT
    
public:
    DiskLayerModel(std::string path, QObject *parent = Q_NULLPTR);
    //~DiskLayerModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
//     QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
//     QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
//     int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
//     int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
//     Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
//     QStringList mimeTypes() const Q_DECL_OVERRIDE;
//     QMimeData* mimeData(const QModelIndexList& indexes) const Q_DECL_OVERRIDE;
//     bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;
//     bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) Q_DECL_OVERRIDE;
    std::vector<std::string> filenames() const;
    
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
/*    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;  */
    void setNewRootPath(std::string path, bool clearFileState);
    void setFileState(std::string name, bool loaded);
    void renameActiveFile(std::string oldName, std::string newName);
    
private slots:
    void resizeFileState();

private:
    bool mEdit = false;
    bool mEnable = false;
    bool mDirectoryLoaded = false;
    std::vector<bool> mFileState;
    
    std::map<std::string, bool> mActiveFiles;
//     std::vector<std::pair<std::string, bool>> mCachedFileState;
};

#endif
