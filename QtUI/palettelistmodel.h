#ifndef PALETTELISTMODEL_H
#define PALETTELISTMODEL_H

#include "palette.h"

#include <QAbstractItemModel>
#include <vector>

class ResourceManager;

class MyQtPalette: public QObject
{
    Q_OBJECT

public:
    MyQtPalette(): MyQtPalette(nullptr) {}
    MyQtPalette(Palette* palette) { mPalette = palette; }
    MyQtPalette(const MyQtPalette& other) { mPalette = other.palette(); }
    
    Palette* palette() const { return mPalette; }
    
private:    
    Palette *mPalette;    
};

Q_DECLARE_METATYPE(MyQtPalette)
Q_DECLARE_METATYPE(Palette*)
Q_DECLARE_OPAQUE_POINTER(Palette*)

//Q_DECLARE_METATYPE(Palette)

class PaletteListModel: public QAbstractListModel
{
    Q_OBJECT
    
public:
    PaletteListModel(ResourceManager* manager);
    ~PaletteListModel();

    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    //QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    //virtual QModelIndex parent(const QModelIndex& child) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    const std::vector<Palette*> palettes() const { return mPalettes; }
    const std::vector<Palette*> localPalettes() const { return mLocalPalettes; }
    
    void populateList(const std::vector<Palette *>& list);
    void populateLocalList(const std::vector<Palette*>& list);
    void addPalette(Palette* palette);
    void delPalette(int row);
    void deleteLocalPalette(int row);
    void addLocalPalette(Palette* palette);
    void clearLocalPalettes() { mLocalPalettes.clear(); }
    void setCurrentLayerType(bool discrete);
    
private:
    ResourceManager* mResourceManager;
    std::vector<Palette *> mPalettes;
    std::vector<Palette*> mLocalPalettes;
    bool mCurrentLayerType;
};

#endif // PALETTELISTMODEL_H
