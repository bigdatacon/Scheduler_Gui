#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    msPlot = new QCustomPlot(this);
    jsPlot = new QCustomPlot(this);

    ui->verticalLayout->addWidget(msPlot);
    ui->verticalLayout->addWidget(jsPlot);

    setupPlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlots()
{
    plotMSOperations();
    plotJSOperations();

    connect(msPlot, &QCustomPlot::plottableClick, this, &MainWindow::handleBarDrag);
}

void MainWindow::plotMSOperations()
{
    for (const auto &ops : ms_operations)
    {
        QCPBars *bar = new QCPBars(msPlot->xAxis, msPlot->yAxis);
        QVector<double> ticks, duration;
        for (const auto &op : ops)
        {
            ticks << op->startTime;
            duration << (op->endTime - op->startTime);
        }
        bar->setData(ticks, duration);
    }
    msPlot->rescaleAxes();
    msPlot->replot();
}

void MainWindow::plotJSOperations()
{
    for (const auto &ops : js_operations)
    {
        QCPBars *bar = new QCPBars(jsPlot->xAxis, jsPlot->yAxis);
        QVector<double> ticks, duration;
        for (const auto &op : ops)
        {
            ticks << op->startTime;
            duration << (op->endTime - op->startTime);
        }
        bar->setData(ticks, duration);
    }
    jsPlot->rescaleAxes();
    jsPlot->replot();
}

void MainWindow::syncPlots(QCPBars *source, QCPBars *target)
{
    QVector<double> keys;
    QVector<double> values;
    for (auto it = source->data()->constBegin(); it != source->data()->constEnd(); ++it)
    {
        keys.append(it->key);
        values.append(it->value);
    }
    target->setData(keys, values);
}

void MainWindow::handleBarDrag(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event)
{
    QCPBars* bar = qobject_cast<QCPBars*>(plottable);
    if (bar)
    {
        double delta = msPlot->xAxis->pixelToCoord(event->pos().x()) - msPlot->xAxis->pixelToCoord(event->pos().x());
        QVector<double> keys;
        QVector<double> values;
        for (auto it = bar->data()->constBegin(); it != bar->data()->constEnd(); ++it)
        {
            keys.append(it->key + delta);
            values.append(it->value);
        }
        bar->setData(keys, values);
        msPlot->replot();
        syncPlots(bar, qobject_cast<QCPBars *>(jsPlot->plottable(0)));
        jsPlot->replot();
    }
}
