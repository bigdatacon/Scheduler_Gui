#ifndef BARCHARTVIEW_H
#define BARCHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QJsonArray>

QT_CHARTS_USE_NAMESPACE

class BarChartView : public QChartView {
    Q_OBJECT
public:
    BarChartView(QWidget *parent = nullptr);
    void loadData(const QJsonArray &operations);

private:
    QChart *chart;
    QBarCategoryAxis *axisY;
    QValueAxis *axisX;
};

#endif // BARCHARTVIEW_H
