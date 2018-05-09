#ifndef PALETTEVALUEWIDGET_H
#define PALETTEVALUEWIDGET_H

#include "balloonwidget.h"
#include "databasetabledataview.h"

#include <QLineEdit>
#include <QPushButton>
#include <memory>
#include <utility>

class BalloonMessageDialog;
class Palette;
class DataBaseTableDataModel;

class PaletteValueWidget : public BalloonWidget
{
    Q_OBJECT
    
public:    
    PaletteValueWidget(Palette* palette, DataBaseTableDataModel* tableModel, int xLimit, const std::pair<int, int>& range, QWidget *parent = Q_NULLPTR);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    double value() const;
    Palette* palette() const { return mPalette; }
    
    void setPalette(Palette* palette);
    void setMaxCoordX(int coordX);
    void setRange(const std::pair<int, int>& range);
    void setCoordinates(int coordX, const std::pair<int, int>& rangeY);
    void setNewValue(double value);
    bool setIntValidator(int minValue, int maxValue);
    bool setDoubleValidator(double minValue, double maxValue);
    bool setControlPointValidator();
    
    void setRowViewVisible(bool visible);
    void setRowViewColumnVisibility(const std::vector<bool>& columnVisibility);
    void move(int x, int y);
    void move(const QPoint &point);

public slots:
    void resizeWithSizeHint();
    void updatePaintingData();
    void updateBasedOnPercent(double percent);
    void updateTableRow();
    void setModelDirty();
    
signals:
    void paletteChanged();
    void paintingValueChanged(double value);
//     void paintingValueChanged(double value);
//     void paintingColorChanged(const QColor& color);
    
protected:
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    
protected slots:
    void showErrorMessage(const QString& text);
    void hideErrorMessage();
    
private:
    enum RowState: int
    {
        Hide = 0,
        Show = 1,
        Edit = 2,
        Commit = 3,
        Discard = 4
    };
    
    void updateBasedOnValue();
    void updateBasedOnPosition(int pos, bool sendSignal);
    
    void editRow();
    void commitEdit();
    void discardEdit();
    
    void adjustGeometry();

    std::unique_ptr<BalloonMessageDialog> mErrorDialog;
    std::unique_ptr<QLineEdit> mEditor;
    std::unique_ptr<DataBaseTableDataView> mTableRowView;
    std::unique_ptr<QPushButton> mEditRowButton;
    std::unique_ptr<QPushButton> mCommitEditButton;
    std::unique_ptr<QPushButton> mDiscardEditButton;
    std::vector<int> mTempHiddenColumns;
    bool mTableRowViewVisible = false;
    RowState mRowState = RowState::Hide;
    bool mEditRow = false;
    bool mTableModelChanged = false;

    Palette* mPalette = nullptr;
    int mXLimit;
    std::pair<int, int> mGlobalRange;
    std::pair<int, int> mRange;
    QColor mColor;
    double mValue;

    bool mDragging = false;
    bool mValid = true;
    QPoint mLastPos {0, 0};
    QString mLastText = "";
};

#endif // PALETTEVALUEWIDGET_H

