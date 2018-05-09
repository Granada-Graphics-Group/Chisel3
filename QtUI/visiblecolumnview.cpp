#include "visiblecolumnview.h"
#include "visiblecolumnmodel.h"

#include <QMouseEvent>

VisibleColumnView::VisibleColumnView(QWidget *parent)
:
    QTableView(parent)
{
    
}


// *** Protected Methods *** //

void VisibleColumnView::mousePressEvent(QMouseEvent* event)
{
    auto index = indexAt(event->pos());
    auto columnModel = static_cast<VisibleColumnModel*>(model());    

    if(index.isValid() && columnModel->primaryColumn() != index.column())    
    {
        if(event->button() == Qt::RightButton)
        {
            if (columnModel->primaryColumn() == 0)
            {
                columnModel->setSecondaryColumn(index.column());

//                 if (columnModel->isSecondaryColumnSelected(index.column()))
//                     selectColumn(index.column());
//                 else
//                     selectionModel()->select(index, QItemSelectionModel::Deselect);
            }
        }
        else
        {
            columnModel->setPrimary(index.column());
            
/*            auto previous = columnModel->index(0, columnModel->primaryColumn());
            selectionModel()->select(previous, QItemSelectionModel::Deselect);
            selectColumn(index.column()); */       
        }
    }
}


// *** Public slots *** //

void VisibleColumnView::clearSelection()
{
    auto columnModel = static_cast<VisibleColumnModel*>(model());
    columnModel->clearColumns();
    QTableView::clearSelection();
}

