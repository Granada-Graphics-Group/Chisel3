#include "palettetableview.h"
#include "palettemodel.h"
#include "balloonmessagedialog.h"

#include <glm/glm.hpp>
#include <QHeaderView>
#include <QScrollBar>
#include <QMouseEvent>
#include <QColorDialog>


PaletteTableView::PaletteTableView(QWidget* parent)
:
    QTableView(parent)
{
    verticalHeader()->setVisible(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    ValidatedDelegate* lineDelegate = new ValidatedDelegate();
    
    setItemDelegateForColumn(0, lineDelegate);
    
    connect(this, &PaletteTableView::doubleClicked, this, &PaletteTableView::editColor);
    connect(lineDelegate, &ValidatedDelegate::invalidInput, this, &PaletteTableView::showErrorMessage);
    connect(lineDelegate, &ValidatedDelegate::validInput, this, &PaletteTableView::hideErrorMessage);
    connect(lineDelegate, &ValidatedDelegate::duplicatedInput, this, &PaletteTableView::duplicatedMessage);
    connect(lineDelegate, &ValidatedDelegate::cancelEdit, this, &PaletteTableView::cancelEdition);
    
    mErrorDialog = std::make_unique<BalloonMessageDialog>(this);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setTipOrientation(BalloonDialog::TipOrientation::Right);
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed);    
}

PaletteTableView::~PaletteTableView()
{

}


// *** Public Methods *** //

void PaletteTableView::setModel(QAbstractItemModel* model)
{    
    QTableView::setModel(model);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}


// *** Public Slots *** //

void PaletteTableView::addControlPoint()
{
    if (mAddControlPointState)
        delControlPoint();

    auto customModel = dynamic_cast<PaletteModel *>(model());    
    customModel->addControlPoint();
    setCurrentIndex(customModel->index(customModel->rowCount() - 1, 0));
    edit(currentIndex());

    mAddControlPointState = true;

    emit controlPointCountChanged();
}

void PaletteTableView::delControlPoint()
{
    auto customModel = dynamic_cast<PaletteModel *>(model());
    auto tempIndex = currentIndex();
    customModel->deleteControlPoint(currentIndex());
    
    if(tempIndex.row() > 0)
//     {
//         if(tempIndex.row() < customModel->rowCount())
        setCurrentIndex(customModel->index(tempIndex.row() - 1, 0));
//         else
//             setCurrentIndex(customModel->index(customModel->rowCount() - 1, 0));
//     }
    else
        setCurrentIndex(customModel->index(0, 0));
    
    emit controlPointCountChanged();
}

void PaletteTableView::toggleInterpolation(bool value)
{
    auto customModel = dynamic_cast<PaletteModel *>(model()); 
    customModel->toggleIntepolation(value);
}

void PaletteTableView::editColor(const QModelIndex& index)
{
    if(index.column() == 1)
    {
        auto color = model()->data(index, Qt::BackgroundRole).value<QColor>();      
        auto newColor = QColorDialog::getColor(color, this->parentWidget(), "Select Color:", QColorDialog::ShowAlphaChannel);
        if(newColor.isValid())
        {
            auto customModel = dynamic_cast<PaletteModel *>(model());
            auto tempValue = customModel->value(index.row());
            model()->setData(index, newColor);
            auto newCurrentIndex = customModel->indexOfValue(tempValue);
            setCurrentIndex(newCurrentIndex);
        }
    }
}

void PaletteTableView::changeCurrentControlPoint(const QModelIndex& current, const QModelIndex &previous)
{
    auto customModel = dynamic_cast<PaletteModel *>(model());
    if(!customModel->containInf())
        emit newCurrentControlPoint(customModel->value(current.row()));
}


// *** Protected Slots *** //

void PaletteTableView::closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{   
    auto customModel = dynamic_cast<PaletteModel *>(model());
    
    if(mCancelEdtion != true)
    {        
        if(mDuplicatedControlPointState)
        {
            mDuplicatedControlPointState = false;
            edit(currentIndex());
        }
        else if(customModel->containInf())
        {
            edit(currentIndex());
        }
        else
        {
            QTableView::closeEditor(editor, hint);
            
            if(mAddControlPointState)
            {
                mAddControlPointState = false;
                editColor(customModel->index(customModel->lastUpdatedValueRow(), 1));
            }
        }
        
        //setCurrentIndex(customModel->index(customModel->lastUpdatedValueRow(), 0));
    }
    else
    {
        customModel->deleteInf();
        mErrorDialog->hide();
        mCancelEdtion = false;
    }
}

void PaletteTableView::showErrorMessage(const QString& text)
{    
    mErrorDialog->setMessage(text, Qt::white);

    auto posY = rowViewportPosition(currentIndex().row()) - mErrorDialog->height() / 2 + rowHeight(currentIndex().row()) / 2 + horizontalHeader()->height();
    auto posX = -mErrorDialog->width();

    mErrorDialog->move(mapToGlobal(QPoint(posX, posY)));
    mErrorDialog->show();
}

void PaletteTableView::hideErrorMessage()
{
    if(mErrorDialog->isVisible()) mErrorDialog->hide();
}

void PaletteTableView::duplicatedMessage(const QString& text)
{
    mDuplicatedControlPointState = true;
    showErrorMessage(text);
}

void PaletteTableView::cancelEdition()
{
    mCancelEdtion = true;
}


// *** Protected Methods *** //

// void PaletteTableView::mousePressEvent(QMouseEvent* event)
// {
// 
// }
// 
// void PaletteTableView::mouseMoveEvent(QMouseEvent* event)
// {
// 
// }
// 
// void PaletteTableView::mouseReleaseEvent(QMouseEvent* event)
// {
// 
// }
// 
// void PaletteTableView::mouseDoubleClickEvent(QMouseEvent* event)
// {
//    QTableView::mouseDoubleClickEvent(event); 
// }
// 
// void PaletteTableView::keyPressEvent(QKeyEvent* event)
// {
// 
// }
