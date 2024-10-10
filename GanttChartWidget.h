#ifndef GANTTCHARTWIDGET_H
#define GANTTCHARTWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include "GanttChart.h"
#include <QToolBar>
#include <QPushButton>  // Добавляем этот include для кнопки
#include <QTextEdit>  // Добавляем для текстового поля
#include <QDialog>    // Для диалогового окна



class GanttChartWidget : public QWidget {
    Q_OBJECT

public:
    GanttChartWidget(QWidget *pParent = nullptr);
    ~GanttChartWidget();
    void LoadJsonData(const QString &sFilename);
    void LoadJsonData_2(const QString &sFilename);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void handleRightClick(QPoint clickPos, std::vector<SJobOperation> &jsOperations, std::vector<SMachineOperation> &msOperations);

    void updateChart();
    void onWorkersTimeButtonClicked();

private slots:
    void OnSolveButtonClicked();  // Обработчик для нажатия кнопки
    void OnSolveButtonClicked_2();  // Обработчик для нажатия кнопки
    void OnShowScheduleMetricsClicked();  // Новый слот для открытия окна с данными


private:
    void Initialize();
    void DrawGanttChart();
    void DrawWorkersTimeChart();
    void UpdateBar(int iJob, int iMachine, int iNewStart, int iNewJob);

    GanttChart* m_pGanttChart;
    QImage m_oChartImage;
    QImage m_oWorkersImage;

    QPoint m_qDragStartPosition;
    int m_iDraggedJob;
    int m_iDraggedMachine;
    bool m_bJsMode;

    QToolBar* m_pToolBar;  // Добавляем тулбар
    QPushButton* m_pSolveButton;  // Кнопка для запуска солвера
    QPushButton* m_pShowTimeButton; // кнопка отображения времени по рабочим

    bool m_bDisplayingWorkersTimeChart = false; // Flag to track which chart to display
    // Добавляем кнопку для отображения метрик расписания
    QPushButton* m_pShowMetricsButton;
    ScheduleMetrics m_ScheduleMetrics;  // Добавляем объект структуры

};

#endif // GANTTCHARTWIDGET_H
