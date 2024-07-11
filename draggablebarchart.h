#ifndef DRAGGABLEBARCHART_H
#define DRAGGABLEBARCHART_H

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QMouseEvent>

QT_CHARTS_USE_NAMESPACE

class DraggableBarChart : public QChartView {
    Q_OBJECT
public:
    DraggableBarChart(QChart *chart, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QBarSet* findBarSet(const QPointF &point);

    QBarSet *m_barSet;
    QPointF m_startPos;
};

#endif // DRAGGABLEBARCHART_H
