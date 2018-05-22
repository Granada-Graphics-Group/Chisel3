#ifndef DATABASETABLESCHEMAVIEW_H
#define DATABASETABLESCHEMAVIEW_H

#include <QTableView>

class DataBaseTableSchemaModel;

class DataBaseTableSchemaView : public QTableView
{
    Q_OBJECT
    
public:
    DataBaseTableSchemaView(DataBaseTableSchemaModel* schemaModel, QWidget* parent = Q_NULLPTR);
    ~DataBaseTableSchemaView();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    
public slots:
    void deleteSelectedField();
    void resizeTableView();
    
protected slots:    
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) override;    
    
private:
    
};

#endif // DATABASETABLESCHEMAVIEW_H
