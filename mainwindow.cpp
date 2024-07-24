#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      customPlot(new QCustomPlot(this)),
      startTimeEdit(new QLineEdit(this)),
      finishTimeEdit(new QLineEdit(this)),
      jobEdit(new QLineEdit(this)),
      machineEdit(new QLineEdit(this)),
      applyButton(new QPushButton("Apply", this)),
      resetButton(new QPushButton("Reset", this)),
      layout(new QVBoxLayout)
{

    setCentralWidget(centralWidget); // Установка центрального виджета
    setupPlot();

    layout->addWidget(customPlot);
    layout->addWidget(new QLabel("Start Time:", this));
    layout->addWidget(startTimeEdit);
    layout->addWidget(new QLabel("Finish Time:", this));
    layout->addWidget(finishTimeEdit);
    layout->addWidget(new QLabel("Job:", this));
    layout->addWidget(jobEdit);
    layout->addWidget(new QLabel("Machine:", this));
    layout->addWidget(machineEdit);
    layout->addWidget(applyButton);
    layout->addWidget(resetButton);

    centralWidget->setLayout(layout);

    connect(applyButton, &QPushButton::clicked, this, &MainWindow::onApplyButtonClicked);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
}

MainWindow::~MainWindow() {}

QWidget* MainWindow::getCentralWidget() {
    return centralWidget;
}

void MainWindow::setupPlot() {
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->xAxis->setRange(0, 10);
    customPlot->yAxis->setRange(0, 10);

    // Create QCPBars for machines and jobs
    machinesBars = new QCPBars(customPlot->yAxis, customPlot->xAxis);
    jobsBars = new QCPBars(customPlot->yAxis, customPlot->xAxis);

    // Set bars width
    machinesBars->setWidth(0.5);
    jobsBars->setWidth(0.5);

    // Initialize data for the bars
    QVector<double> machinesData, jobsData, ticks;
    machinesData << 1 << 2 << 3 << 4 << 5;
    jobsData << 1 << 2 << 3 << 4 << 5;
    ticks << 1 << 2 << 3 << 4 << 5;

    // Set data for the bars
    machinesBars->setData(ticks, machinesData);
    jobsBars->setData(ticks, jobsData);

    // Configure y-axis text labels
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (int i = 0; i < ticks.size(); ++i) {
        textTicker->addTick(ticks[i], QString("Label %1").arg(i+1));
    }
    customPlot->yAxis->setTicker(textTicker);
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel("Machines/Jobs");

    customPlot->replot();
}

void MainWindow::updateBars() {
    // This function should include the logic to update the bars and related bars on the other graph according to the input values
}

void MainWindow::onApplyButtonClicked() {
    // Apply changes
    updateBars();
}

void MainWindow::onResetButtonClicked() {
    // Reset to the original view
    customPlot->xAxis->setRange(0, 10);
    customPlot->yAxis->setRange(0, 10);
    updateBars();
}
