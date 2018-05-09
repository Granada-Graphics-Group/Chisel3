#ifndef ACTIVELAYERMODEL_H
#define ACTIVELAYERMODEL_H

#include "activelayeritem.h"

#include <QAbstractItemModel>
#include <tuple>
#include <memory>

class QMimeData;
class DiskLayerModel;


class ActiveLayerModel : public QAbstractItemModel
{
    Q_OBJECT
    
public:
    ActiveLayerModel(DiskLayerModel* diskLayerModel, QObject *parent = Q_NULLPTR);
    ~ActiveLayerModel();
        
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    QMimeData* mimeData(const QModelIndexList& indexes) const Q_DECL_OVERRIDE;
    bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) Q_DECL_OVERRIDE;
    
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    
    void addLayer(const QString name, bool isRegisterType, std::pair<int, int> resolution, const std::string type);
    void insertLayer(unsigned int row, const QString name, bool isRegisterType, std::pair<int, int> resolution, const std::string type);
    void removeLayer(int row);
    void removeLayers(int firstRow, int lastRow);
    void addLayerValues(const QString name, const QList< QPair< QColor, QStringList > >& values1);
    void addLayerValues(const QString& name, const QList< std::tuple<int, QColor, QStringList > >& values);
    
    bool updateLayerValues(const QString& name, const QList< std::tuple<int, int, QColor, QStringList > >& values);       
    bool removeLayerValues(const QString& name, const QList<int>& values);
    
    void setVisibililty(int row, bool visibility);
    
signals:
    void layerRenamed(const QString& newName);
    void layerPositionChanged(int lastIndex, int newIndex);
    
private:
    void setupModelData(const QStringList& lines, ActiveLayerItem* parent);

    std::unique_ptr<ActiveLayerItem> mRootItem;
    DiskLayerModel* mDiskLayerModel;
};

#endif // ACTIVELAYERMODEL_H
