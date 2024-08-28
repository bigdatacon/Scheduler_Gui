#include "MainWindow.h"
#include <QVBoxLayout>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    m_pChartWidget = new GanttChartWidget(this);
    setCentralWidget(m_pChartWidget);
}

