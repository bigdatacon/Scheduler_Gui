#ifndef GANTTCHARTMAINWINDOW_H
#define GANTTCHARTMAINWINDOW_H

#include <QMainWindow>
#include "GanttChartWidget.h"

class GanttChartMainWindow : public QMainWindow {
    Q_OBJECT

public:
    GanttChartMainWindow(QWidget *parent = nullptr);

    void LoadJsonData(const QString &filename) {
        m_pChartWidget->LoadJsonData(filename);
    }

private:
    GanttChartWidget *m_pChartWidget;
};

#endif // GANTTCHARTMAINWINDOW_H
