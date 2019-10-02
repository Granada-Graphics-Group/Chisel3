#ifndef PALETTEMODEL_H
#define PALETTEMODEL_H

#include <QAbstractItemModel>

class Palette;

class PaletteModel : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    PaletteModel(Palette* palette = nullptr);
    ~PaletteModel();
    //PaletteModel& operator=(const PaletteModel& other);
    Palette* palette() const { return mPalette; }
    double value(int index) const;
    QModelIndex indexOfValue(double value) const;
    
    double lastUpdatedValue() const { return mLastUpdatedValue;  }
    double lastUpdatedValueRow() const;
    bool containInf() const;

    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    //virtual QModelIndex parent(const QModelIndex& child) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;    
    void setPalette(Palette* palette);
    void addControlPoint();
    void deleteControlPoint(const QModelIndex& controlPointIndex);
    void deleteInf();
    void toggleIntepolation(bool value);
    
signals:
    void interpolationChanged();

private:
    Palette* mPalette;
    bool mAddControlPointState = false;
    double mLastUpdatedValue;
};

#endif // PALETTEMODEL_H
