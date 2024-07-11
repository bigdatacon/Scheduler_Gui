#include "barchartview.h"
#include <QJsonArray>
#include <QJsonObject>

BarChartView::BarChartView(QWidget *parent)
    : QChartView(new QChart(), parent), chart(QChartView::chart())
{
    chart->setTitle("Gantt Chart Example");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    axisX = new QValueAxis();
    axisX->setRange(0, 300);  // Adjust as per your data range
    axisX->setTitleText("Time");

    axisY = new QBarCategoryAxis();
    axisY->setTitleText("Machine");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
}

void BarChartView::loadData(const QJsonArray &operations)
{
    QMap<int, QBarSeries*> machineSeriesMap;
    QStringList categories;

    for (const QJsonValue &value : operations) {
        QJsonObject obj = value.toObject();
        int machine = obj["Machine"].toInt();
        int job = obj["Job"].toInt();
        int start = obj["Start"].toInt();
        int finish = obj["Finish"].toInt();

        if (!machineSeriesMap.contains(machine)) {
            QBarSeries *series = new QBarSeries();
            machineSeriesMap[machine] = series;
            chart->addSeries(series);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }

        QBarSet *set = new QBarSet(QString::number(job));
        *set << (finish - start);
        machineSeriesMap[machine]->append(set);

        if (!categories.contains(QString::number(machine))) {
            categories << QString::number(machine);
        }
    }

    axisY->append(categories);
}
