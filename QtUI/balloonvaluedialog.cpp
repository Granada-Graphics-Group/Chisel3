#include "balloonvaluedialog.h"
#include "databasetabledatamodel.h"
#include "databaserowdataview.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>

BalloonValueDialog::BalloonValueDialog(DataBaseTableDataModel* tableModel, QWidget* parent)
:
    BalloonMessageDialog(parent)
{
    tableModel->setHeaderVisible(false);
    tableModel->setEditable(false);
    mTableRowView = std::make_unique<DataBaseRowDataView>(tableModel, true, this);
    mTableRowView->setFrameShape(QFrame::NoFrame);
    mTableRowView->horizontalHeader()->hide();
    mTableRowView->verticalHeader()->hide();
    mTableRowView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTableRowView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mTableRowView->hide();

    layout()->addWidget(mTableRowView.get());
    //layout()->setSizeConstraint(QLayout::SetFixedSize);
}


// *** Public Methods *** //

QSize BalloonValueDialog::sizeHint() const
{
    auto margins = layout()->contentsMargins();
    if(mShowDataBaseRow)
        return mTableRowView->sizeHint() + QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    else
        return message()->sizeHint() + QSize(margins.left() + margins.right() + 20, margins.top() + margins.bottom() + 10);
}

void BalloonValueDialog::showValue(float value, const QColor & color)
{
    mTableRowView->hide();
    message()->show();
    setMessage(QString::number(value), Qt::white);
    mShowDataBaseRow = false;
    adjustGeometry(false);
    resize(sizeHint());
    adjustSize();
}

void BalloonValueDialog::showRowValue(int rowIndex)
{
    auto model = static_cast<DataBaseTableDataModel*>(mTableRowView->model());
    model->setIdFilter(QString::number(rowIndex));
    message()->hide();
    mTableRowView->show();
    mShowDataBaseRow = true;
    adjustGeometry(true);
    resize(sizeHint());
    adjustSize();
}


// *** Private Methods *** //

void BalloonValueDialog::adjustGeometry(bool rowVisible)
{
    int defaultMargins = 0;
    if(rowVisible) defaultMargins = 5;
        
    switch(tipOrientation())
    {
        case BalloonDialog::TipOrientation::Up:
            layout()->setContentsMargins(defaultMargins, defaultMargins + tipHeight(), defaultMargins, defaultMargins);
            break;
        case BalloonDialog::TipOrientation::Left:
            layout()->setContentsMargins(defaultMargins + tipHeight(), defaultMargins, defaultMargins, defaultMargins);
            break;
        case BalloonDialog::TipOrientation::Right:
            layout()->setContentsMargins(defaultMargins, defaultMargins, tipHeight(), defaultMargins);
            break;
        case BalloonDialog::TipOrientation::Down:
            layout()->setContentsMargins(defaultMargins, defaultMargins, defaultMargins, tipHeight() + defaultMargins);
            break;            
    }
    
        
}

