#ifndef GANTTCHARTWIDGET_H
#define GANTTCHARTWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include "GanttChart.h"

#include <QToolBar>
#include <QPushButton>  // Добавляем этот include для кнопки

class GanttChartWidget : public QWidget {
    Q_OBJECT

public:
    GanttChartWidget(QWidget *pParent = nullptr);
    ~GanttChartWidget();
    void LoadJsonData(const QString &sFilename);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void OnSolveButtonClicked();  // Обработчик для нажатия кнопки

private:
    void Initialize();
    void DrawGanttChart();
    void UpdateBar(int iJob, int iMachine, int iNewStart, int iNewJob);

    GanttChart* m_oLogic;
    QImage m_oChartImage;
    QPoint m_qDragStartPosition;
    int m_iDraggedJob;
    int m_iDraggedMachine;
    bool m_bJsMode;

    QToolBar* m_pToolBar;  // Добавляем тулбар
    QPushButton* m_pSolveButton;  // Кнопка для запуска солвера

};

#endif // GANTTCHARTWIDGET_H
