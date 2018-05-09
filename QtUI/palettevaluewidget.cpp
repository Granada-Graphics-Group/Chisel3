#include "palettevaluewidget.h"
#include "databasetabledatamodel.h"
#include "tableproxyfilter.h"
#include "intvalidatorreporter.h"
#include "doublevalidatorreporter.h"
#include "controlpointvalidator.h"
#include "balloonmessagedialog.h"
#include "palette.h"
#include "logger.hpp"

#include <QLineEdit>
#include <QFontMetrics>
#include <QAction>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <limits>
#include <algorithm>

PaletteValueWidget::PaletteValueWidget(Palette* palette, DataBaseTableDataModel* tableModel, int xLimit, const std::pair< int, int >& range, QWidget* parent)
:
    BalloonWidget(parent),
    mPalette(palette),
    mXLimit(xLimit),
    mGlobalRange(range),
    mRange(range)
{
    setTipOrientation(TipOrientation::Right);

    setMouseTracking(true);
    setWindowModality(Qt::NonModal);
  
    mEditor = std::make_unique<QLineEdit>(this);
    mEditor->setObjectName(QStringLiteral("Editor"));
    mEditor->setAlignment(Qt::AlignCenter);
    mEditor->setStyleSheet("background-color: white;");
    mEditor->setMinimumWidth(30);
    mEditor->setMaximumWidth(110);
    mEditor->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    mEditor->installEventFilter(this);

    if(mPalette->type() == Palette::Type::Discrete)
        mEditor->setText(QString::number(static_cast<int>(mPalette->minValue())));
    else
        mEditor->setText(QString::number(mPalette->minValue()));

//     auto proxy = new TableProxyFilter(tableModel, this);
//     proxy->useHorizontalHeader(false);
//     proxy->setVisibleColumns({1, 2});
    tableModel->setHeaderVisible(false);
    mTableRowView = std::make_unique<DataBaseTableDataView>(tableModel, true, this);
    mTableRowView->setHiddenColumns({0});
    mTableRowView->setFrameShape(QFrame::NoFrame);
    //mTableRowView->setModel(proxy);
    mTableRowView->horizontalHeader()->hide();
    mTableRowView->verticalHeader()->hide();
    mTableRowView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //mTableRowView->horizontalHeader()->setMaximumSectionSize(80);
    mTableRowView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mTableRowView->hide();
        
    mEditRowButton = std::make_unique<QPushButton>(this);
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/edit.png"), QSize(), QIcon::Normal, QIcon::Off);
    mEditRowButton->setIcon(icon);
    mEditRowButton->setObjectName(QStringLiteral("EditRow"));
    mEditRowButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mEditRowButton->setMinimumWidth(30);
    mEditRowButton->setMaximumWidth(30);
    mEditRowButton->setAutoFillBackground(false);
    mEditRowButton->setFlat(true);
    mEditRowButton->hide();
    
    mCommitEditButton = std::make_unique<QPushButton>(this);
    QIcon icon2;
    icon2.addFile(QStringLiteral(":/icons/accept.png"), QSize(), QIcon::Normal, QIcon::Off);
    mCommitEditButton->setIcon(icon2);
    mCommitEditButton->setObjectName(QStringLiteral("CommitEdit"));
    mCommitEditButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mCommitEditButton->setMaximumWidth(30);
    mCommitEditButton->setAutoFillBackground(false);
    mCommitEditButton->setFlat(true);
    mCommitEditButton->hide();
    
    mDiscardEditButton = std::make_unique<QPushButton>(this);
    QIcon icon3;
    icon3.addFile(QStringLiteral(":/icons/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
    mDiscardEditButton->setIcon(icon3);
    mDiscardEditButton->setObjectName(QStringLiteral("DiscardEdit"));
    mDiscardEditButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mDiscardEditButton->setMaximumWidth(30);
    mDiscardEditButton->setAutoFillBackground(false);
    mDiscardEditButton->setFlat(true);
    mDiscardEditButton->hide();
        
    mErrorDialog = std::make_unique<BalloonMessageDialog>(qobject_cast< QWidget* >(this->parent()));
    mErrorDialog->setTipOrientation(BalloonMessageDialog::TipOrientation::Right);
    mErrorDialog->setWindowFlags(mErrorDialog->windowFlags() | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    mErrorDialog->setAttribute(Qt::WA_ShowWithoutActivating);
    mErrorDialog->setBackgroundColor(Qt::darkRed); 

    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horizontalLayout->addWidget(mEditRowButton.get());
    horizontalLayout->addWidget(mTableRowView.get());
    horizontalLayout->addWidget(mCommitEditButton.get());
    horizontalLayout->addWidget(mDiscardEditButton.get());
    horizontalLayout->addWidget(mEditor.get());
    //horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(horizontalLayout);

    connect(mEditor.get(), &QLineEdit::textEdited, this, &PaletteValueWidget::resizeWithSizeHint);
    connect(mEditor.get(), &QLineEdit::editingFinished, this, &PaletteValueWidget::updateBasedOnValue);
    connect(mEditRowButton.get(), &QPushButton::released, this, &PaletteValueWidget::editRow);
    connect(mCommitEditButton.get(), &QPushButton::released, this, &PaletteValueWidget::commitEdit);
    connect(mDiscardEditButton.get(), &QPushButton::released, this, &PaletteValueWidget::discardEdit);
    
    connect(tableModel, &DataBaseTableDataModel::dataChanged, this, &PaletteValueWidget::setModelDirty);

    updateBasedOnValue();
}


// *** Public Methods *** //

QSize PaletteValueWidget::sizeHint() const
{    
    QFontMetrics fm(mEditor->font());
    auto fontWidth = fm.width(mEditor->text());
    
    int width = 0;
    
    if(mDragging)
       width += mEditor->maximumWidth();
    else
    {
        if(mEditor->minimumWidth() < fontWidth)
            width += fontWidth + 10;
        else
            width += mEditor->minimumWidth();
    }
    
    auto margins = layout()->contentsMargins();
    
    //LOG("Pre Margins Width: ", width, " Font Width: ", fontWidth);
    
    width += layout()->contentsMargins().left() + layout()->contentsMargins().right();
    
    //LOG("Post Margins Width: ", width, " Font Width: ", fontWidth);
    
    if(mRowState > Hide)
    {
        width += (mRowState != Edit) ? (mEditRowButton->sizeHint().width() + layout()->spacing()) : (mCommitEditButton->sizeHint().width() + mDiscardEditButton->sizeHint().width() + 2 * layout()->spacing());
        
        if(mTableRowView->isVisible()) 
            width += mTableRowView->sizeHint().width() + layout()->spacing();
    }

    //LOG("Post Margins Row Width: ", width);    
    
    int height = 42;
    
    return QSize(width, height);
}

double PaletteValueWidget::value() const
{
    return mEditor->text().toDouble();
}

void PaletteValueWidget::setPalette(Palette* palette)
{
    mPalette = palette;
    updatePaintingData();
}

void PaletteValueWidget::setMaxCoordX(int coordX)
{
    mXLimit = coordX;
    mLastText = "";
    updateBasedOnValue();
}

void PaletteValueWidget::setRange(const std::pair<int, int>& range)
{
    mRange = range;
    mLastText = "";
    updateBasedOnValue();
}

void PaletteValueWidget::setCoordinates(int coordX, const std::pair<int, int>& rangeY)
{
    mXLimit = coordX;
    mGlobalRange = rangeY;
    adjustGeometry();
    
    mLastText = "";
    updateBasedOnValue();    
}


void PaletteValueWidget::setNewValue(double value)
{
    mValue = value;
    //bool paletteEdited = false;
    //if(mValue > mPalette->maxValue())
    //{
    //    mPalette->addControlPoint(mValue, mPalette->color(mPalette->maxValue()));
    //    paletteEdited = true;
    //}
    //else if(mValue < mPalette->minValue())
    //{
    //    mPalette->addControlPoint(mValue, mPalette->color(mPalette->minValue()));
    //    paletteEdited = true;
    //}  
    //
    //double percent;

    double percent;
    bool outerValue = false;

    if (mValue > mPalette->maxValue())
    {
        percent = 1.0;
    }
    else if (mValue < mPalette->minValue())
    {
        percent = 0.0;
    }
    else if (mPalette->isInterpolating())
    {
        if (mPalette->type() == Palette::Type::Discrete)
        {
            //mEditor->setText(QString::number(static_cast<int>(value)));
            if (mRowState > Hide)
            {
                LOG("New Value");

                updateTableRow();
            }
            percent = static_cast<double>(value + 0.5 - mPalette->minValue()) / static_cast<double>(mPalette->maxValue() + 1 - mPalette->minValue());
        }
        else
        {
            //mEditor->setText(QString::number(value));
            percent = static_cast<double>(value - mPalette->minValue()) / static_cast<double>(mPalette->maxValue() - mPalette->minValue());
        }
    }
    else
    {
        //if(mPalette->type() == Palette::Type::Discrete)
        //    mEditor->setText(QString::number(static_cast<int>(value)));
        //else
        //    mEditor->setText(QString::number(value));

        auto controlPoints = mPalette->controlPoints();
        auto foundControPoint = controlPoints.find(mValue);

        if (foundControPoint != end(controlPoints))
        {
            auto index = std::distance(begin(controlPoints), foundControPoint) + 0.5;
            percent = index / controlPoints.size();
        }
        else
        {
            auto values = mPalette->controlPointValues();
            auto upper = std::upper_bound(begin(values), end(values), mValue);

            percent = static_cast<double>(upper - begin(values)) / controlPoints.size();
        }
    }

    if (mPalette->type() == Palette::Type::Discrete)
        mEditor->setText(QString::number(static_cast<int>(value)));
    else
        mEditor->setText(QString::number(value));

    mLastText = mEditor->text();

    auto y = static_cast<int>(ceil(percent * (mRange.second - mRange.first) + mRange.first));
    move(x(), y);

    auto color = mPalette->color(value);
    mColor = QColor::fromRgbF(color.r, color.g, color.b, color.a);
    setBackgroundColor(mColor);

    //if(paletteEdited)
    //    emit paletteChanged();
    //
    resize(sizeHint());
}


bool PaletteValueWidget::setIntValidator(int minValue, int maxValue)
{   
    auto minL = std::numeric_limits<int>::min();
    auto maxL = std::numeric_limits<int>::max();
/*    
    if(minValue >= std::numeric_limits<int>::min() && maxValue <= std::numeric_limits<int>::max())
    {*/
        //auto validator = std::make_unique<IntValidatorReporter>(minValue, maxValue);
        auto validator = new IntValidatorReporter(minL, maxL);
        mEditor->setValidator(validator);
        connect(validator, &IntValidatorReporter::rejected, this, &PaletteValueWidget::showErrorMessage);
        connect(validator, &IntValidatorReporter::validated, this, &PaletteValueWidget::hideErrorMessage);

        return true;
/*        
        return true;
    }
    else
        return false;*/
}

bool PaletteValueWidget::setDoubleValidator(double minValue, double maxValue)
{
    auto minL = std::numeric_limits<double>::lowest();
    auto maxL = std::numeric_limits<double>::max();
//     if(minValue >= std::numeric_limits<double>::lowest() && maxValue <= std::numeric_limits<double>::max())
//     {
        //auto validator = std::make_unique<DoubleValidatorReporter>(minValue, maxValue);
        auto validator = new DoubleValidatorReporter(minL, maxL);
        mEditor->setValidator(validator);
        connect(validator, &DoubleValidatorReporter::rejected, this, &PaletteValueWidget::showErrorMessage);
        connect(validator, &DoubleValidatorReporter::validated, this, &PaletteValueWidget::hideErrorMessage);
        
        return true;
/*        
        return true;
    }
    else
        return false;*/
}

bool PaletteValueWidget::setControlPointValidator()
{
    //mEditor->setValidator(0);
//     auto validator = new ControlPointValidator(mPalette);
//     mEditor->setValidator(validator);
//     connect(validator, &ControlPointValidator::rejected, this, &PaletteValueWidget::showErrorMessage);
//     connect(validator, &ControlPointValidator::validated, this, &PaletteValueWidget::hideErrorMessage);
    
    return true;
}


void PaletteValueWidget::setRowViewVisible(bool visible)
{
    if(visible)
    {
        //mRowState = Show;
        mRowState = Hide;
        setDisabled(true);
    }
    else
    {        
        mRowState = Hide;
        setEnabled(true);
    }
    
    mTempHiddenColumns.clear();

    updateTableRow();
}

void PaletteValueWidget::setRowViewColumnVisibility(const std::vector<bool>& columnVisibility)
{
    std::vector<int> hidden;

    for(auto i = 0; i < columnVisibility.size(); ++i)
        if(!columnVisibility[i])
            hidden.push_back(i);
    
    mTableRowView->setHiddenColumns(hidden);
    resize(sizeHint());
}


// *** Public Slots *** //

void PaletteValueWidget::resizeWithSizeHint()
{
    resize(sizeHint());
}


void PaletteValueWidget::updatePaintingData()
{
    updateBasedOnPosition(y(), true);
    update();
}

void PaletteValueWidget::updateBasedOnPercent(double percent)
{
    if(mPalette->type() == Palette::Type::Discrete)
    {
        if(mPalette->isInterpolating())
            mValue = static_cast<int>(std::min({mPalette->maxValue(), percent * (mPalette->maxValue() + 1 - mPalette->minValue()) + mPalette->minValue()}));
        else
        {
            auto controlPointIndex = static_cast<int>(std::floor(percent * mPalette->controlPoints().size()));
            mValue = static_cast<int>(mPalette->controlPoint(controlPointIndex).first);
        }

        mEditor->setText(QString::number(static_cast<int>(mValue)));
        if(mRowState > Hide && mValue != mLastText.toDouble())
        {
            LOG("Updated BasedOnPercent");
            updateTableRow();
        }
    }
    else
    {
        if(mPalette->isInterpolating())
            mValue = percent * (mPalette->maxValue() - mPalette->minValue()) + mPalette->minValue();
        else
        {
            auto controlPointIndex = static_cast<int>(std::floor(percent * mPalette->controlPoints().size()));
            mValue = mPalette->controlPoint(controlPointIndex).first;
        }

        mEditor->setText(QString::number(mValue));
    }
    
    mLastText = mEditor->text();

    auto y = percent * (mRange.second - mRange.first) + mRange.first;    
    move(x(), y); 
    
    auto color = mPalette->color(mValue);
    mColor = QColor::fromRgbF(color.r, color.g, color.b, color.a);
    setBackgroundColor(mColor);
        
    emit paintingValueChanged(mValue);
    
    resize(sizeHint());
}

void PaletteValueWidget::updateTableRow()
{
    auto model = static_cast<DataBaseTableDataModel *>(mTableRowView->model());
        
    switch(mRowState)
    {
        case Hide:
        {
            model->setIdFilter(mEditor->text());
            
            mEditRowButton->hide();
            mDiscardEditButton->hide();
            mCommitEditButton->hide();
            mTableRowView->hide();

            mTableModelChanged = false;
            break;
        }
        case Show:
        {
            model->setIdFilter(mEditor->text());
            
            mEditRowButton->show();            
            mDiscardEditButton->hide();
            mCommitEditButton->hide();
            
            (model->rowCount() == 0) ? mTableRowView->hide() : mTableRowView->show();

            mTableModelChanged = false;
            break;            
        }
        case Edit:
        {
            model->setIdFilter(mEditor->text());
            
            if(mTableRowView->isHidden()) mTableRowView->show(); 

            mEditRowButton->hide();        
            mCommitEditButton->show();
            mDiscardEditButton->show();
            
            if(mTempHiddenColumns.size() == 0)
            {
                mTempHiddenColumns = mTableRowView->hiddenColumns();
                mTableRowView->showHiddenColumns();
                mTableRowView->setHiddenColumns({0});
            }
            
            model->setHeaderVisible(true);
            
            if(model->rowCount() == 0)
                model->addRow(static_cast<int>(mValue));

            mTableModelChanged = false;            
            break;
        }
        case Commit:
        {
            if(mTableModelChanged)
            {
                model->setData(model->index(0,0), mValue);
                model->commitData();
        
                mTableModelChanged = false;
            }
        }
        case Discard:
        {
            model->setIdFilter(mEditor->text());
            
            mCommitEditButton->hide();
            mDiscardEditButton->hide();    
            mEditRowButton->show();        
            
            model->setHeaderVisible(false);        
            mTableRowView->horizontalHeader()->hide();
            mTableRowView->setHiddenColumns(mTempHiddenColumns);    
            mTempHiddenColumns.clear();
            
            (model->rowCount() == 0) ? mTableRowView->hide() : mTableRowView->show();
            
            mRowState = Show;
            break;
        }
    }
    
//    (model->rowCount() == 0) ? mTableRowView->hide() : mTableRowView->show();
    
    resize(sizeHint());      
}

void PaletteValueWidget::setModelDirty()
{
    if(mRowState == Edit)
    {
        mTableModelChanged = true;
        resize(sizeHint());
    }
}


// *** Protected Methods *** //

void PaletteValueWidget::mousePressEvent(QMouseEvent* event)
{
    mDragging = true;
    mLastPos =  event->globalPos();

    //QWidget::mousePressEvent(event);
}

void PaletteValueWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(mDragging)
    {
        
        const QPoint delta = event->globalPos() - mLastPos;
        QPoint newPos(x(), y() + delta.y());
        
        if(y() + delta.y() < mRange.first)
            newPos.setY(mRange.first);
        else if(y() + delta.y() > mRange.second)
            newPos.setY(mRange.second);

        move(newPos);
        
        LOG("DRAGGING:: Pos y: ", newPos.y());
        
        updateBasedOnPosition(newPos.y(), false);
        
        mLastPos = event->globalPos();
    }

    //QWidget::mouseMoveEvent();
}

void PaletteValueWidget::mouseReleaseEvent(QMouseEvent* event)
{
    mDragging = false;
    
    emit paintingValueChanged(mValue);
    
    resize(sizeHint());
    //QWidget::mouseReleaseEvent(event);
}

void PaletteValueWidget::resizeEvent(QResizeEvent* event)
{
    adjustGeometry();
    
    BalloonWidget::resizeEvent(event);
    
    move(x(), y());
}

bool PaletteValueWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == mEditor.get())
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            
            if(keyEvent->key() == Qt::Key_Escape)
                mEditor->setText(mLastText);
        }
        else if(event->type() == QEvent::FocusOut)
        {
            if(mValid == false)
            {
                mEditor->setText(mLastText);
                mValid = true;
            }
            else if(mEditor->text().isEmpty())
                mEditor->setText(mLastText);
        }
    }
    
    return false;
}


// *** Protected slots *** //

void PaletteValueWidget::showErrorMessage(const QString& text)
{
    mErrorDialog->setMessage(text, Qt::white);

    mErrorDialog->move(mapToGlobal(QPoint(-mErrorDialog->width(), height()/2 - mErrorDialog->height()/2)));    
    mErrorDialog->show();
}

void PaletteValueWidget::hideErrorMessage()
{
    if(mErrorDialog->isVisible()) mErrorDialog->hide();
}


// *** Private Methods *** //

void PaletteValueWidget::updateBasedOnValue()
{
    //mValid = true;
    //
    //if(!mPalette->isInterpolating())
    //{
    //    ControlPointValidator validator(mPalette);
    //    auto text = mEditor->text();
    //    auto pos = 0;
    //    auto validation = validator.validate(text, pos);
    //    if (validation == QValidator::Acceptable)
    //        hideErrorMessage();
    //    else
    //    {
    //        mValid = false;
    //        showErrorMessage("The number is not a valid control point");
    //        mEditor->setFocus();
    //    }
    //}

    if(!mEditor->text().isEmpty() && QString::compare(mLastText, mEditor->text()) != 0)
    {
        mLastText = mEditor->text();
        bool valueChanged = mValue != mLastText.toDouble();
        mValue = mLastText.toDouble();

        double percent;
        bool outerValue = false;

        if(mValue > mPalette->maxValue())
        {
            percent = 1.0;
        }
        else if(mValue < mPalette->minValue())
        {
            percent = 0.0;
        }
        else if (mPalette->isInterpolating())
        {
            if (mPalette->type() == Palette::Type::Discrete)
            {
                percent = static_cast<double>(mValue + 0.5 - mPalette->minValue()) / static_cast<double>(mPalette->maxValue() + 1 - mPalette->minValue());
                if (mRowState > Hide && valueChanged)
                {
                    LOG("Updated BasedOnValue");
                    updateTableRow();
                }
            }
            else
                percent = static_cast<double>(mValue - mPalette->minValue()) / static_cast<double>(mPalette->maxValue() - mPalette->minValue());
        }
        else
        {
            auto controlPoints = mPalette->controlPoints();
            auto foundControPoint = controlPoints.find(mValue);

            if(foundControPoint != end(controlPoints))
            {
                auto index = std::distance(begin(controlPoints), foundControPoint) + 0.5;
                percent = index / controlPoints.size();
            }
            else
            {
                auto values = mPalette->controlPointValues();
                auto upper = std::upper_bound(begin(values), end(values), mValue);

                percent = static_cast<double>(upper - begin(values))/controlPoints.size();
            }
        }

        //percent = std::min(std::max(percent, 0.0), 1.0);

        auto y = static_cast<int>(ceil(percent * (mRange.second - mRange.first) + mRange.first));
        move(x(), y);

        auto color = mPalette->color(mValue);
        mColor = QColor::fromRgbF(color.r, color.g, color.b, color.a);
        setBackgroundColor(mColor);

        //if(paletteEdited)
        //    emit paletteChanged();
        emit paintingValueChanged(mValue);
        
        resize(sizeHint());
    }
}

void PaletteValueWidget::updateBasedOnPosition(int pos, bool sendSignal)
{
    auto percent = static_cast<double>(pos - mRange.first) / static_cast<double>(mRange.second - mRange.first);

    if(!mPalette->isInterpolating())
    {
        mValue = static_cast<int>(mPalette->controlPoint(static_cast<int>(std::floor(percent/(1.0/mPalette->controlPoints().size())))).first);
        mEditor->setText(QString::number(mValue));
    }
    else
    {
        if(mPalette->type() == Palette::Type::Discrete)
        {
            if(mPalette->isInterpolating())
                mValue = static_cast<int>(std::min({mPalette->maxValue(), percent * (mPalette->maxValue() + 1 - mPalette->minValue()) + mPalette->minValue()}));
            else
            {
                auto controlPointIndex = static_cast<int>(std::floor(percent * mPalette->controlPoints().size()));
                mValue = static_cast<int>(mPalette->controlPoint(controlPointIndex).first);                
            }
            
            mEditor->setText(QString::number(static_cast<int>(mValue)));
            if(mRowState > Hide && mValue != mLastText.toInt())
            {
                LOG("Updated BasedOnPosition");
                
                updateTableRow();
            }
        }
        else
        {
            if(mPalette->isInterpolating())
                mValue = percent * (mPalette->maxValue() - mPalette->minValue()) + mPalette->minValue();
            else
            {
                auto controlPointIndex = static_cast<int>(std::floor(percent * mPalette->controlPoints().size()));
                mValue = mPalette->controlPoint(controlPointIndex).first;                
            }
            
            mEditor->setText(QString::number(mValue));
        }
    }
    mLastText = mEditor->text();
    
    auto color = mPalette->color(mValue);
    mColor = QColor::fromRgbF(color.r, color.g, color.b, color.a);
    setBackgroundColor(mColor);
    
    if(sendSignal) emit paintingValueChanged(mValue);
    
    resize(sizeHint());
}

void PaletteValueWidget::editRow()
{
    mRowState = Edit;
    
    updateTableRow();
    
    resize(sizeHint());
}

void PaletteValueWidget::commitEdit()
{
    mRowState = Commit;
        
    updateTableRow();
    
    resize(sizeHint());
}

void PaletteValueWidget::discardEdit()
{
    mRowState = Discard;

    updateTableRow();
            
    resize(sizeHint());    
}


void PaletteValueWidget::adjustGeometry()
{
    
    switch(tipOrientation())
    {
        case TipOrientation::Up:
            layout()->setContentsMargins(0, tipHeight(), 0, 0);
            break;
        case TipOrientation::Left:
            layout()->setContentsMargins(tipHeight(), 0, 0, 0);
            break;
        case TipOrientation::Right:       
            layout()->setContentsMargins(5, 5, 5 + tipHeight(), 5);
            break;
        case TipOrientation::Down:
            layout()->setContentsMargins(0, 0, 0, tipHeight());
            break;            
    }
    
    mRange = {mGlobalRange.first - ceil(height()/2.0), mGlobalRange.second - ceil(height()/2.0)};
}

void PaletteValueWidget::move(int x, int y)
{
    LOG("Move:: X original: ", x, ", X actualizada: ", mXLimit - width(), ", Y: ", y);
    if(x + width() != mXLimit)
        x = mXLimit - width();
    BalloonWidget::move(x, y);    
}


void PaletteValueWidget::move(const QPoint& point)
{
    LOG("Move::Point:: X original: ", point.x(), ", X actualizada: ", mXLimit - width(), ", Y: ", point.y());
    LOG("Width: ", width(), " Height: ", height());
    QPoint newPoint = point;
    if(point.x() + width() != mXLimit)
        newPoint.setX(mXLimit - width());

    BalloonWidget::move(newPoint);
}
