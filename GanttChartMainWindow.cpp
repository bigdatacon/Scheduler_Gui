#include "GanttChartMainWindow.h"
#include <QVBoxLayout>



GanttChartMainWindow::GanttChartMainWindow(QWidget *parent)
    : QMainWindow(parent) {
    m_pChartWidget = new GanttChartWidget(this);
    setCentralWidget(m_pChartWidget);
}

