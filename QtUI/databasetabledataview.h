#ifndef DATABASETABLEDATAVIEW_H
#define DATABASETABLEDATAVIEW_H

#include "customtableview.h"
#include <memory>

class DataBaseTableDataModel;
class DataBaseTableDataDelegate;

class DataBaseTableDataView : public CustomTableView
{
    Q_OBJECT
    
public:
    DataBaseTableDataView(DataBaseTableDataModel* dataModel, bool resizeToContents, QWidget* parent = Q_NULLPTR);
    ~DataBaseTableDataView();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    const std::vector<int>& hiddenColumns() const { return mHiddenColumns; }
    
    void setHiddenColumns(const std::vector<int>& columns);
    void showHiddenColumns();

signals:
    void newIdSelected(int id);
    
public slots:
    bool selectNewId(int id);
    void selectNewIndex(const QModelIndex& index);
    void updateColumnVisibility();
    void updateDelegates();
    
protected:
    void leaveEvent(QEvent * event) override;
//     virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command);
//     virtual QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers);

private:
    std::unique_ptr<DataBaseTableDataDelegate> mDataDelegate;
    std::vector<int> mHiddenColumns;
    bool mResizeToContents;
};

#endif // DATABASETABLEDATAVIEW_H
