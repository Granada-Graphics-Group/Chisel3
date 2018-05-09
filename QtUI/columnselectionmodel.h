#ifndef COLUMNSELECTIONMODEL_H
#define COLUMNSELECTIONMODEL_H

#include <QItemSelectionModel>

class RegisterLayer;

class ColumnSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
    
public:    
    ColumnSelectionModel(QAbstractItemModel *model = Q_NULLPTR);
    
    void setLayer(RegisterLayer* layer){ mLayer = layer; }
    
private slots:
    void updateVisibleColumns(const QItemSelection &selected, const QItemSelection &deselected);
    
private:
    RegisterLayer* mLayer;    
};    

#endif
