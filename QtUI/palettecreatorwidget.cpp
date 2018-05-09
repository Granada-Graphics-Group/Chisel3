#include "palettecreatorwidget.h"
#include "palettemodel.h"
#include "balloonmessagedialog.h"
#include "resourcemanager.h"

PaletteCreatorWidget::PaletteCreatorWidget(QWidget *parent)
:
    QWidget(parent)
{
    mUi = std::make_unique<Ui::PaletteCreatorWidget>();
    mUi->setupUi(this);

    mErrorDialog = std::make_unique<BalloonMessageDialog>(this);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setTipOrientation(BalloonDialog::TipOrientation::Up);    
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed);    
        
    connect(mUi->paletteTableView, &PaletteTableView::controlPointCountChanged, this, &PaletteCreatorWidget::updateButtonUsability);
    connect(mUi->nameEdit, &QLineEdit::editingFinished, this, &PaletteCreatorWidget::validateName);
    connect(mUi->acceptButton, &QPushButton::released, this, &PaletteCreatorWidget::finish);
    connect(mUi->cancelButton, &QPushButton::released, this, &PaletteCreatorWidget::discard);
}

PaletteCreatorWidget::~PaletteCreatorWidget()
{

}


// *** Public Methods *** //

PaletteTableView* PaletteCreatorWidget::paletteTableView()
{
    return mUi->paletteTableView;
}

void PaletteCreatorWidget::setState(State state)
{
    mState = state;

    if (mState == State::Create)
    {
        mUi->nameEdit->setText("");
        mUi->acceptButton->setText("Create");
    }
    else
    {
        auto model = static_cast<PaletteModel*>(mUi->paletteTableView->model());
        mUi->nameEdit->setText(QString::fromStdString(model->palette()->name()));

        mUi->acceptButton->setText("Edit");
        mUi->acceptButton->setEnabled(true);
    }
}


// *** Public Slots *** //

void PaletteCreatorWidget::updateButtonUsability()
{
    auto controlPointCount = mUi->paletteTableView->model()->rowCount();
    if(controlPointCount == 2)
        mUi->delCtrlPointButton->setEnabled(false);
    else if(controlPointCount > 2)
        mUi->delCtrlPointButton->setEnabled(true);
}

// *** Private Slots *** //

void PaletteCreatorWidget::validateName()
{
    auto paletteNames = mResourceManager->paletteNames();
    auto found = std::end(paletteNames);

    if(mState == Create)
        found = std::find_if(std::begin(paletteNames), std::end(paletteNames), [&](const std::string& paletteName) { return (paletteName == mUi->nameEdit->text().toStdString()) ? true : false; });
    else
    {
        auto editedPalette = static_cast<PaletteModel*>(mUi->paletteTableView->model())->palette();
        found = std::find_if(std::begin(paletteNames), std::end(paletteNames), [&](const std::string& paletteName) { return (paletteName != editedPalette->name() && paletteName == mUi->nameEdit->text().toStdString()) ? true : false; });
    }

    if(found == std::end(paletteNames))
    {
        mUi->acceptButton->setEnabled(true);        
        if(mErrorDialog->isVisible()) mErrorDialog->hide();
        mResourceManager->palettes().back()->setName(mUi->nameEdit->text().toStdString());
    }
    else
    {
        mUi->acceptButton->setDisabled(true);
        mErrorDialog->setMessage("The palette already exists", Qt::white);
        mErrorDialog->adjustSize();
        mErrorDialog->show();
        mErrorDialog->move(this->mapToGlobal(mUi->nameEdit->pos() + QPoint(0, mUi->nameEdit->height())));        
    }    
}

void PaletteCreatorWidget::finish()
{
    mUi->nameEdit->clear();
    if (mState == Create)
        emit paletteCreated();
    else
        emit paletteEdited();
}

void PaletteCreatorWidget::discard()
{
    mUi->nameEdit->clear();
    if (mState == Create)
        emit paletteDiscarded();
    else
        emit editCanceled();
}
