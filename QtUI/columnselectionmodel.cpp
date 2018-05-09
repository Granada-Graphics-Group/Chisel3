#include "columnselectionmodel.h"
#include "registerlayer.h"

#include <algorithm>
#include <set>

ColumnSelectionModel::ColumnSelectionModel(QAbstractItemModel* model)
:
    QItemSelectionModel(model)
{        
    connect(this, &QItemSelectionModel::selectionChanged, this, &ColumnSelectionModel::updateVisibleColumns);
}


// *** Private Slots *** //

void ColumnSelectionModel::updateVisibleColumns(const QItemSelection& selected, const QItemSelection& deselected)
{
    std::vector<int> visible;
    std::vector<int> hidden;
    
    hidden.resize(model()->columnCount());
    
    std::iota(begin(hidden), end(hidden), 0);
        
    for(const auto& index: selected.indexes())
    {
        auto search = std::find(begin(hidden), end(hidden), index.column() + 1);
        if(search != end(hidden)) hidden.erase(search);
    }
}
