#ifndef COLUMNVISIBILITYMODEL
#define COLUMNVISIBILITYMODEL

#include <QAbstractListModel>
#include <vector>
#include <string>

class RegisterLayer;

class ColumnVisibilityModel : public QAbstractListModel
{
public:
    ColumnVisibilityModel(QObject *parent = Q_NULLPTR);
    
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;
    const std::vector<bool> columnVisibility() const;
    bool isDirty() const { return mDirty; } 
        
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    void setLayer(RegisterLayer* layer);
    void setColumnVisibility(const std::vector<bool>& columnVisibility);
    void applyChanges();
    void updateVisibilityWithSourceLayer();
    
private:
    std::vector<bool> mNewColumnVisibility;
    RegisterLayer* mLayer;
    bool mDirty = false;
};

#endif
