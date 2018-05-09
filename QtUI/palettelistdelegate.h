#ifndef PALETTELISTDELEGATE_H
#define PALETTELISTDELEGATE_H

#include <QStyledItemDelegate>

class Palette;

class PaletteListDelegate: public QStyledItemDelegate
{
    Q_OBJECT    
    
public:
    PaletteListDelegate(QObject* parent = Q_NULLPTR);
    ~PaletteListDelegate();
    
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const Q_DECL_OVERRIDE;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const Q_DECL_OVERRIDE;
    
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const Q_DECL_OVERRIDE;

protected slots:
    void onEditorDestroyed(QObject* obj);    
    
private:
    void paintGradient(QPainter* painter, const QRect& rect, Palette* palette, double colorHeight) const;
    
    mutable bool mEditing = false;
};

#endif // PALETTELISTDELEGATE_H
