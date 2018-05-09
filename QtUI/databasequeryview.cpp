#include "databasequeryview.h"

#include <QHeaderView>

DataBaseQueryView::DataBaseQueryView(QWidget * parent)
:
    QTableView(parent)
{
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);

    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
}
