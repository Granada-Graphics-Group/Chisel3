#include "paletteeditorwidget.h"

PaletteEditorWidget::PaletteEditorWidget(QWidget *parent)
:
    QWidget(parent)    
{
    mUi = std::make_unique<Ui::PaletteEditorWidget>();
    mUi->setupUi(this);

    connect(mUi->tableViewPalette, &PaletteTableView::controlPointCountChanged, this, &PaletteEditorWidget::updateButtonUsability);
}

PaletteEditorWidget::~PaletteEditorWidget()
{

}


// *** Public Methods *** //

PaletteTableView* PaletteEditorWidget::paletteTableView()
{
    return mUi->tableViewPalette;
}


// *** Public Slots *** //

void PaletteEditorWidget::updateButtonUsability()
{
    auto controlPointCount = mUi->tableViewPalette->model()->rowCount();
    if(controlPointCount == 2)
        mUi->buttonDelCtrlPoint->setEnabled(false);
    else if(controlPointCount > 2)
        mUi->buttonDelCtrlPoint->setEnabled(true);
}

