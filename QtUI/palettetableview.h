#ifndef PALETTETABLEVIEW_H
#define PALETTETABLEVIEW_H

#include "validateddelegate.h"

#include <QTableView>
#include <memory>

class BalloonMessageDialog;

class PaletteTableView :  public QTableView
{
    Q_OBJECT

public:
    PaletteTableView(QWidget *parent = Q_NULLPTR);
    ~PaletteTableView();

    ValidatedDelegate* lineDelegate() { return static_cast<ValidatedDelegate*>(itemDelegateForColumn(0)); };
    void setModel(QAbstractItemModel * model) Q_DECL_OVERRIDE;

signals:
    void newCurrentControlPoint(double value);
    void controlPointCountChanged();
    
public slots:
    void addControlPoint();
    void delControlPoint();
    void toggleInterpolation(bool value);
    void editColor(const QModelIndex& index);
    void changeCurrentControlPoint(const QModelIndex& current, const QModelIndex &previous);
    
protected slots:    
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) Q_DECL_OVERRIDE;
    void showErrorMessage(const QString& text);
    void hideErrorMessage();
    void duplicatedMessage(const QString& text);
    void cancelEdition();
    
protected:
//     virtual void mousePressEvent(QMouseEvent* event);
//     virtual void mouseMoveEvent(QMouseEvent* event);
//     virtual void mouseReleaseEvent(QMouseEvent* event);
//     virtual void mouseDoubleClickEvent(QMouseEvent* event);
//     virtual void keyPressEvent(QKeyEvent* event);
private:
    bool mAddControlPointState = false;
    bool mDuplicatedControlPointState = false;
    bool mCancelEdtion = false;
    std::unique_ptr<BalloonMessageDialog> mErrorDialog;
};

#endif // PALETTETABLEVIEW_H
