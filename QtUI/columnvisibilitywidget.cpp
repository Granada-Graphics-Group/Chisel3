#include "columnvisibilitywidget.h"
#include "columnvisibilitymodel.h"

#include <QListView>
#include <QPushButton>
#include <QHBoxLayout>

ColumnVisibilityWidget::ColumnVisibilityWidget(ColumnVisibilityModel* model, QWidget* parent)
:
    QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    
    auto view = new QListView(this);
    view->setModel(model);
    view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    
    mainLayout->addWidget(view);
    
    mApplyVisibilityButton = new QPushButton("Apply", this);
    mApplyVisibilityButton->setObjectName(QStringLiteral("applyButton"));
    mApplyVisibilityButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    
    mainLayout->addWidget(mApplyVisibilityButton);
    
    setLayout(mainLayout);
}

