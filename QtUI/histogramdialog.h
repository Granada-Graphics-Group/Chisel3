#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QtCharts/QChart>

QT_CHARTS_USE_NAMESPACE

class Chisel;
class QLineEdit;
class QCheckBox;

class HistogramDialog : public QDialog
{
    Q_OBJECT
    
public:
    HistogramDialog(Chisel* chisel, unsigned int layerIndex, QWidget* parent = Q_NULLPTR);

    
public slots:    
    void setIntervalCount();

private:
    Chisel* mChisel;
    unsigned int mLayerIndex;
    std::vector<std::array<float, 2>> mAreaPerValue;
    std::vector<std::array<QVariant, 2>> mAreaPerInterval;
    QChart* mChart;
    QCheckBox* mUseIntervals;    
    QLineEdit* mIntervalCountEditor;
};

#endif
