#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    msPlot(new QCustomPlot(this)),
    jsPlot(new QCustomPlot(this))
{
    ui->setupUi(this);

    // Sample data
    ms_operations = {
        {new MSOperation{0, 1, 71, 102}, new MSOperation{0, 1, 169, 204}},
        {new MSOperation{0, 2, 102, 129}, new MSOperation{0, 2, 129, 169}},
        {new MSOperation{2, 4, 0, 38}, new MSOperation{0, 4, 38, 71}, new MSOperation{1, 4, 71, 109}, new MSOperation{2, 4, 109, 173}},
        {new MSOperation{2, 5, 65, 107}},
        {new MSOperation{1, 6, 14, 35}, new MSOperation{2, 6, 38, 65}, new MSOperation{2, 6, 227, 254}},
        {new MSOperation{1, 7, 0, 14}},
        {new MSOperation{1, 8, 109, 150}, new MSOperation{2, 8, 173, 227}}
    };

    js_operations = {
        {new JSOperation{1, 3, 38, 71}, new JSOperation{1, 0, 71, 102}, new JSOperation{1, 1, 102, 129}, new JSOperation{1, 1, 129, 169}, new JSOperation{1, 0, 169, 204}},
        {new JSOperation{2, 6, 0, 14}, new JSOperation{2, 5, 14, 35}, new JSOperation{2, 3, 71, 109}, new JSOperation{2, 7, 109, 150}},
        {new JSOperation{3, 3, 0, 38}, new JSOperation{3, 5, 38, 65}, new JSOperation{3, 4, 65, 107}, new JSOperation{3, 3, 109, 173}, new JSOperation{3, 7, 173, 227}, new JSOperation{3, 5, 227, 254}}
    };

    setupPlots();
    plotMSOperations();
    plotJSOperations();

    connect(msPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(handleBarDrag(QCPAbstractPlottable*,QMouseEvent*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlots()
{
    auto layout = new QVBoxLayout;
    layout->addWidget(msPlot);
    layout->addWidget(jsPlot);
    ui->centralwidget->setLayout(layout);
}

void MainWindow::plotMSOperations()
{
    for (const auto& ops : ms_operations) {
        QCPBars *bar = new QCPBars(msPlot->xAxis, msPlot->yAxis);
        QVector<double> ticks, start, duration;
        for (const auto& op : ops) {
            ticks << op->machineId;
            start << op->startTime;
            duration << (op->endTime - op->startTime);
        }
        bar->setData(ticks, duration);
        msPlot->addPlottable(bar);
    }
    msPlot->rescaleAxes();
    msPlot->replot();
}

void MainWindow::plotJSOperations()
{
    for (const auto& ops : js_operations) {
        QCPBars *bar = new QCPBars(jsPlot->xAxis, jsPlot->yAxis);
        QVector<double> ticks, start, duration;
        for (const auto& op : ops) {
            ticks << op->jobId;
            start << op->startTime;
            duration << (op->endTime - op->startTime);
        }
        bar->setData(ticks, duration);
        jsPlot->addPlottable(bar);
    }
    jsPlot->rescaleAxes();
    jsPlot->replot();
}

void MainWindow::syncPlots(QCPBars *source, QCPBars *target)
{
    // Implement logic to sync target plot based on source plot's new data
    target->setData(source->data()->keys(), source->data()->values());
    target->parentPlot()->replot();
}

void MainWindow::handleBarDrag(QCPAbstractPlottable *plottable, QMouseEvent *event)
{
    if (QCPBars *bar = qobject_cast<QCPBars*>(plottable)) {
        double key = bar->parentPlot()->xAxis->pixelToCoord(event->pos().x());
        double value = bar->parentPlot()->yAxis->pixelToCoord(event->pos().y());

        // Adjust bar's data based on mouse drag
        bar->data()->clear();
        bar->addData(key, value);
        bar->parentPlot()->replot();

        // Sync with corresponding bar in jsPlot
        syncPlots(bar, qobject_cast<QCPBars*>(jsPlot->plottable()));
    }
}
