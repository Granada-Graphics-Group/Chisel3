#ifndef DATABASEEDITORWIDGET_H
#define DATABASEEDITORWIDGET_H

#include "balloonwidget.h"
#include "databasetable.h"

#include <memory>

class BalloonMessageDialog;
class DataBaseTableSchemaView;
class DataBaseTableSchemaModel;
class VisibleColumnModel;
class DataBaseTableDataView;
class DataBaseTableDataModel;
class VisibleColumnView;
class ColumnVisibilityWidget;
class ColumnVisibilityModel;
class DataBaseResourceModel;
class DataBaseResourceView;
class VSizedLayout;
class QHBoxLayout;
class QListView;
class QLineEdit;
class QPushButton;

class DataBaseEditorWidget : public BalloonWidget
{
    Q_OBJECT

public:
    DataBaseEditorWidget(DataBaseTableSchemaModel* schemaModel, DataBaseTableDataModel* dataModel, ColumnVisibilityModel* visibilityModel, VisibleColumnModel* columnModel, DataBaseResourceModel* resourceModel, QWidget* parent = Q_NULLPTR);
    ~DataBaseEditorWidget();
    
    QSize sizeHint() const Q_DECL_OVERRIDE;
    
    QPushButton* addFieldButton() const { return mAddFieldButton; }
    QPushButton* deleteFieldButton() const { return mDeleteFieldButton; }
    QPushButton* finishEditingButton() const { return mFinishEditingButton; } 
    
    DataBaseTableDataView* dataBaseTableView() const { return mDataView.get(); }
    DataBaseTableSchemaView* dataBaseSchemaView() const { return mSchemaView.get(); }
    VisibleColumnView* visibleColumnView() const { return mVisibleColumns.get(); }
    
    unsigned int layerIndex() const { return mLayerIndex; }
    
    bool selectId(int id);
    bool selectRow(int row);
    void setLayer(unsigned int layer);
    void setHiddenColumns(const std::vector<int>& columns);
    void setResourceDirectory(const std::string& layerName);
    void clearResourceDirectorySelection();
    
signals:
    void schemaEditingFinished();
    void editingFinished();
    void resourceImported();

public slots:
    void hide();
    void filterTable();
    void clearFilterTable();
    void updateSizes();
    void addRow();
    void deleteRow();
    void finishEditing();
    void cancelEditing();
    void editSchema();
    void selectEditingMode();
    void showErrorInSchemaEditing(const QString& message, int row, int column);
    void hideErrorMessage();    
    void updateColumnVisibility();
    
protected:
    void showEvent(QShowEvent* event) override;
    void moveEvent(QMoveEvent* event) override;

private slots:    
    void finishSchemaEditing();
    void cancelSchemaEditing();
    void updateVisibleColumns();
    void importResource();
    void deleteResource();
    void copyTableToClipboard();
    void toggleColumnVisibilityWidget();
        
private:
    enum class EditingMode : char
    {
        Schema = 0,
        Table = 1
    };

    void setEditingMode(EditingMode newState);
    QPoint errorMessagePosition();
            
    QLineEdit* mFilterLineEdit;
        
    std::unique_ptr<DataBaseTableSchemaView> mSchemaView;
    QWidget* mSchemaEditingWidget;
    QPushButton* mDeleteFieldButton;
    QPushButton* mAddFieldButton;
    QPushButton* mCancelEditingButton;
    QPushButton* mFinishEditingButton;    
    
    std::unique_ptr<DataBaseTableDataView> mDataView;
    QWidget* mDataEditingWidget;
    QPushButton* mEditSchemaButton;    
    QPushButton* mAddRowButton;
    QPushButton* mDelRowButton;
    QPushButton* mFinishTableEditingButton;
    QPushButton* mCancelTableEditingButton;
    QPushButton* mCopyTableToClipboardButton;
    
    std::unique_ptr<VisibleColumnView> mVisibleColumns;
    QHBoxLayout* mVisibleColumnLayout;

    std::unique_ptr<QWidget> mColumnVisibilityWidget;
    std::unique_ptr<QListView> mVisibilityView;
    QPushButton* mOpenVisibilityWidgetButton;
    QPushButton* mApplyVisibilityButton;

    std::unique_ptr<DataBaseResourceView> mResourceView;
    QPushButton* mImportResourceButton;
    QPushButton* mDeleteResourceButton;
    
    //QVBoxLayout* mGlobalVerticalLayout;
    VSizedLayout* mGlobalVerticalLayout;

    std::unique_ptr<BalloonMessageDialog> mErrorDialog;
    std::pair<int, int> mBadEditedCellIndices;
    
    EditingMode mEditingMode;
    DataBaseTable mTempTableSchema;
    std::vector<bool> mTempColumnVisibility;
    bool mEditingSchemaFromTable = false;
    
    unsigned int mLayerIndex;
};

#endif // DATABASEEDITORWIDGET_H
