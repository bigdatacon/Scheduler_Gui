#ifndef GANTTCHARTWIDGET_H
#define GANTTCHARTWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include "GanttChartLogic.h"

class GanttChartWidget : public QWidget {
    Q_OBJECT

public:
    GanttChartWidget(QWidget *pParent = nullptr);
    void LoadJsonData(const QString &sFilename);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void Initialize();
    void DrawGanttChart();
    void UpdateBar(int iJob, int iMachine, int iNewStart, int iNewJob);

    GanttChartLogic m_oLogic;
    QImage m_oChartImage;
    QPoint m_qDragStartPosition;
    int m_iDraggedJob;
    int m_iDraggedMachine;
    bool m_bJsMode;
};

#endif // GANTTCHARTWIDGET_H
