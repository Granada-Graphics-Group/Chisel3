#include "histogramdialog.h"
#include "customtableview.h"
#include "areapervaluemodel.h"
#include "chisel.h"

#include <QHeaderView>
#include <QtCharts>

HistogramDialog::HistogramDialog(Chisel* chisel, unsigned int layerIndex, QWidget* parent)
:
    QDialog(parent),
    mChisel(chisel),
    mLayerIndex(layerIndex)
{
    auto area = mChisel->computeSurfaceArea();
    auto layerArea = mChisel->computeLayerArea(layerIndex);

    QChart* chart = new QChart();
    chart->setTitle("Layer Area");
    chart->setAnimationOptions(QChart::AllAnimations);

    QPieSeries* series = new QPieSeries();
    float a = layerArea/area * 100;
    float b = (area - layerArea)/area * 100;
    *series << new QPieSlice(QString::number(layerArea) + " (" + QString::number(layerArea/area * 100, 'f', 2) + "%)", layerArea);
    *series << new QPieSlice(QString::number(area - layerArea) + " (" + QString::number((area - layerArea)/area * 100, 'f', 2) + "%)", (area - layerArea));
    series->setLabelsVisible(true);
    chart->addSeries(series);

    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->markers()[0]->setLabel("Used");
    chart->legend()->markers()[1]->setLabel("Unused");
    

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // -------- //

    mAreaPerValue = mChisel->computeLayerValueArea(layerIndex);

    QChart* areaPerValueChart = new QChart();
    mChart = areaPerValueChart;
    areaPerValueChart->setTitle("Area Per Value");
    areaPerValueChart->setAnimationOptions(QChart::AllAnimations);

    QStringList categories;
    QBarSet* barset = new QBarSet("Area");
    for (const auto& value : mAreaPerValue)
    {
        categories << QString::number(value[0]);
        *barset << value[1];
    }

    QBarCategoryAxis* axis = new QBarCategoryAxis();
    axis->append(categories);

    QBarSeries* barSeries = new QBarSeries();
    barSeries->append(barset);

    areaPerValueChart->addSeries(barSeries);

    areaPerValueChart->createDefaultAxes();
    areaPerValueChart->setAxisX(axis, barSeries);

    areaPerValueChart->legend()->setVisible(false);
    //areaPerValueChart->legend()->setAlignment(Qt::AlignBottom);

    AreaPerValueModel* tableModel = new AreaPerValueModel(this);
    tableModel->setAreaPerValue(mAreaPerValue, area - layerArea);
    
    CustomTableView* tableView = new CustomTableView(this);
    tableView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);    
    tableView->verticalHeader()->hide();
    tableView->setModel(tableModel);
    
    QChartView* areaPerValueChartView = new QChartView(areaPerValueChart);
    areaPerValueChartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout* areaPerValueLayout = new QHBoxLayout();
    areaPerValueLayout->setContentsMargins(0, 0, 0, 0);
    areaPerValueLayout->addWidget(tableView);
    areaPerValueLayout->addWidget(areaPerValueChartView);

    mUseIntervals = new QCheckBox();
    mUseIntervals->setObjectName(QStringLiteral("useIntervals"));
    mUseIntervals->setText("Use Intervals");

    mIntervalCountEditor = new QLineEdit();
    mIntervalCountEditor->setObjectName(QStringLiteral("IntervalCountEditor"));
    mIntervalCountEditor->setDisabled(true);
    
    QIntValidator* validator = new QIntValidator(1, std::numeric_limits<int>::max());
    mIntervalCountEditor->setValidator(validator);
    
    QHBoxLayout* intervalLayout = new QHBoxLayout();
    intervalLayout->addWidget(mUseIntervals);
    intervalLayout->addWidget(mIntervalCountEditor);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);
    mainLayout->addLayout(areaPerValueLayout);
    mainLayout->addLayout(intervalLayout);
    
    setIntervalCount();

    setLayout(mainLayout);
    
    connect(mUseIntervals, &QCheckBox::toggled, mIntervalCountEditor, &QLineEdit::setEnabled);
    connect(mUseIntervals, &QCheckBox::toggled, this, &HistogramDialog::setIntervalCount);
    connect(mIntervalCountEditor, &QLineEdit::editingFinished, this, &HistogramDialog::setIntervalCount);
}

// *** Public Slots *** //

void HistogramDialog::setIntervalCount()
{
    auto intervalCount = mIntervalCountEditor->text().toInt();
    
    mChart->removeAllSeries();
    mChart->removeAxis(mChart->axisX());
    
    QBarCategoryAxis* axis = new QBarCategoryAxis();
    QBarSeries* barSeries = new QBarSeries();
    
    QStringList categories;
    QBarSet* barset = new QBarSet("Area");    
        
    if(mUseIntervals->isChecked() && intervalCount > 0 && intervalCount < mAreaPerValue.size())
    {        
        auto valuesPerInterval = static_cast<int>(std::floor(mAreaPerValue.size() /static_cast<float>(intervalCount)));

        std::vector<std::pair<QString, float>> areaPerInterval;
        
        for(auto intervalIndex  = 0; intervalIndex < intervalCount; intervalIndex++)
        {
            QString intervalName;
            auto intervalSize = valuesPerInterval;
            auto leftIndex = intervalIndex * valuesPerInterval;
            auto remaining = mAreaPerValue.size() - leftIndex;
        
            if(intervalIndex != (intervalCount - 1))
                intervalName = "[" + QString::number(mAreaPerValue[leftIndex][0]) + " - " + QString::number(mAreaPerValue[leftIndex + valuesPerInterval - 1][0]) + "]";
            else
            {
                intervalSize = remaining;
                intervalName = "[" + QString::number(mAreaPerValue[leftIndex][0]) + " - " + QString::number(mAreaPerValue.back()[0]) + "]";
            }

            areaPerInterval.push_back({intervalName, 0});                
            
            for(auto valueIndex = 0; valueIndex < intervalSize; valueIndex++)
                areaPerInterval[intervalIndex].second += mAreaPerValue[leftIndex + valueIndex][1];
        }
        
        mAreaPerInterval.clear();
        
        for(const auto& intervalPair: areaPerInterval)
        {
            mAreaPerInterval.push_back({{intervalPair.first, intervalPair.second}});
            categories << intervalPair.first;
            *barset << intervalPair.second;            
        }
    }
    else
    {
        for (const auto& value : mAreaPerValue)
        {
            categories << QString::number(value[0]);
            *barset << value[1];
        }        
    }
        
    axis->append(categories);
    barSeries->append(barset);
    mChart->addSeries(barSeries);
    mChart->createDefaultAxes();
    mChart->setAxisX(axis, barSeries);
}

