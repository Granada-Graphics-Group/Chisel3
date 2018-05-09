#ifndef AREAPERVALUEMODEL_H
#define AREAPERVALUEMODEL_H

#include <QAbstractTableModel>
#include <array>

class AreaPerValueModel : public QAbstractTableModel
{
public:
    AreaPerValueModel(QObject* parent = Q_NULLPTR);

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex & parent) const override;
    int columnCount(const QModelIndex & parent) const override;

    void setAreaPerValue(const std::vector<std::array<float, 2>>& areaPerValue, float unusedArea);
    void setAreaPerInterval(const std::vector<std::array<QVariant, 2>>& areaPerValue) { mData = areaPerValue; }
    
private:
    std::vector<std::array<QVariant, 2>> mData;
};

#endif
