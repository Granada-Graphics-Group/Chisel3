#ifndef TABLEPROXYFILTER_H
#define TABLEPROXYFILTER_H

#include <QSortFilterProxyModel>
#include <map>
#include <set>

class TableProxyFilter : public QSortFilterProxyModel
{
public:
    TableProxyFilter(QAbstractItemModel* sourceModel, QObject* parent = Q_NULLPTR);
    
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    
    void useHorizontalHeader(bool useHeader) { mUseHorizontalHeader = useHeader; }
    
    void setColumnEditable(unsigned int index, bool editable);

    void setVisibleRows(const std::vector<int>& rows);
    void setVisibleColumns(const std::vector<int>& columns);
    
protected:
    bool filterAcceptsColumn(int sourceColumn, const QModelIndex & sourceParent) const Q_DECL_OVERRIDE;    
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const Q_DECL_OVERRIDE;
    
private:
    bool mUseHorizontalHeader = true;
    std::map<int, std::pair<bool, Qt::ItemFlags>> mColumnData;
    std::set<int> mVisibleRows;
};

#endif
