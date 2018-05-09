#ifndef PALETTELISTVIEW_H
#define PALETTELISTVIEW_H

#include <QListView>

class PaletteListView: public QListView
{
    Q_OBJECT

public:
    PaletteListView(QWidget* parent = Q_NULLPTR);
    ~PaletteListView();

    void editNewPalette();
    
    bool eventFilter(QObject * watched, QEvent * event) override;
    
signals:
    void newPaletteNameEdited();
    void deleteNewPalette();
// public slots:
//     void seeSelected(const QItemSelection &selected, const QItemSelection &deselected);
    
private slots:
    void updateNewPaletteEditionState();

private:
    bool mEditingNewPalette = false;    
};

#endif // PALETTELISTVIEW_H
