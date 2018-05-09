#include "palettelistview.h"
#include "palettelistmodel.h"
#include "palettelistdelegate.h"


#include <QHeaderView>
#include <QKeyEvent>
#include <QEvent>

PaletteListView::PaletteListView(QWidget* parent)
:
    QListView(parent)
{
    //horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //horizontalHeader()->setStretchLastSection(true);
    //horizontalHeader()->setVisible(false);
    //verticalHeader()->setVisible(false);
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    setSelectionBehavior(QAbstractItemView::SelectRows);
    //setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(SingleSelection);

    setItemDelegate(new PaletteListDelegate(this));
    
    itemDelegate()->installEventFilter(this);
    
    connect(itemDelegate(), &QAbstractItemDelegate::closeEditor, this, &PaletteListView::updateNewPaletteEditionState);
}

PaletteListView::~PaletteListView()
{

}


// *** Public Methods *** //

void PaletteListView::editNewPalette()
{
    mEditingNewPalette = true;
    edit(model()->index(model()->rowCount() - 1, 0));
}

bool PaletteListView::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == itemDelegate())
    {
        if (event->type() == QEvent::KeyPress)
        {
            switch (static_cast<QKeyEvent *>(event)->key()) 
            {
            case Qt::Key_Escape:
                if(mEditingNewPalette == true)
                    emit deleteNewPalette();
                break;
            case Qt::Key_Tab:
                if(mEditingNewPalette == true) 
                    emit deleteNewPalette();
                break;
            }
            
        }
    }
    
    return QListView::eventFilter(watched, event);
}


// *** Private slots *** //

void PaletteListView::updateNewPaletteEditionState()
{
    if(mEditingNewPalette)
    {        
        mEditingNewPalette = false;
        auto palette = qvariant_cast<Palette*>(model()->data(model()->index(model()->rowCount() - 1, 0)));
        emit (palette->name().size() > 0) ? newPaletteNameEdited() : deleteNewPalette();
    }
}

// void PaletteListView::seeSelected(const QItemSelection &selected, const QItemSelection &deselected)
// {
//     std::vector<int> hola;
//     for (auto index : selected.indexes())
//         hola.push_back(index.row());
//     auto a = 2;
//     auto i = 3;    
// }
