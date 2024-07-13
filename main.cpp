#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "qcustomplot.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

struct Operation {
    int job;
    int machine;
    int start;
    int finish;
};

QVector<Operation> jsOperations;
QVector<Operation> msOperations;

QLineEdit *startTimeEdit;
QLineEdit *finishTimeEdit;
QLineEdit *jobEdit;
QLineEdit *machineEdit;

QCPBars *barsJobs;
QCPBars *barsMachines;

QCustomPlot *plot1;
QCustomPlot *plot2;

Operation *selectedOperation = nullptr;

void setupPlot(QCustomPlot *customPlot, const QVector<Operation> &operations, bool groupByJob) {
    QMap<int, int> uniqueLabels;
    QVector<double> yPositions;
    QVector<double> durations;
    QVector<QString> labels;
    int index = 0;

    for (const auto &op : operations) {
        int label = groupByJob ? op.job : op.machine;
        if (!uniqueLabels.contains(label)) {
            uniqueLabels[label] = index++;
        }
        yPositions.append(uniqueLabels[label]);
        durations.append(op.finish - op.start);
        labels.append(QString("%1").arg(groupByJob ? op.machine : op.job));
    }

    QCPBars *bars = new QCPBars(customPlot->yAxis, customPlot->xAxis);
    bars->setData(yPositions, durations);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (auto it = uniqueLabels.begin(); it != uniqueLabels.end(); ++it) {
        textTicker->addTick(it.value(), QString(groupByJob ? "J%1" : "M%1").arg(it.key()));
    }

    customPlot->yAxis->setTicker(textTicker);
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel(groupByJob ? "Jobs" : "Machines");
    customPlot->xAxis->setRange(0, 300);
    customPlot->yAxis->setRange(-1, index);
    customPlot->replot();

    if (groupByJob) {
        barsJobs = bars;
    } else {
        barsMachines = bars;
    }
}

void plotGraphs() {
    plot1->clearPlottables();
    plot2->clearPlottables();

    setupPlot(plot1, jsOperations, false);
    setupPlot(plot2, jsOperations, true);
}

void onBarClicked(QMouseEvent *event, QCustomPlot *customPlot) {
    int index = -1;
    QCPBars *bars = nullptr;

    if (customPlot == plot1) {
        bars = barsMachines;
    } else if (customPlot == plot2) {
        bars = barsJobs;
    }

    if (bars) {
        double y = customPlot->yAxis->pixelToCoord(event->pos().y());
        double key = round(y);
        index = bars->data()->findBegin(key)->key;

        if (index >= 0 && index < jsOperations.size()) {
            selectedOperation = &jsOperations[index];
            startTimeEdit->setText(QString::number(selectedOperation->start));
            finishTimeEdit->setText(QString::number(selectedOperation->finish));
            jobEdit->setText(QString::number(selectedOperation->job));
            machineEdit->setText(QString::number(selectedOperation->machine));
        }
    }
}

void updateBar() {
    if (selectedOperation) {
        selectedOperation->start = startTimeEdit->text().toInt();
        selectedOperation->finish = finishTimeEdit->text().toInt();
        selectedOperation->job = jobEdit->text().toInt();
        selectedOperation->machine = machineEdit->text().toInt();
        plotGraphs();
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow window;

    // Загрузка данных из JSON файла
    std::ifstream file("data.json");
    if (!file) {
        std::cerr << "Error: Unable to open file data.json" << std::endl;
        return 1;
    }

    json data;
    try {
        file >> data;
    } catch (json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return 1;
    }
    file.close();

    try {
        for (const auto &op : data["js_operations"]) {
            jsOperations.append({op["Job"], op["Machine"], op["Start"], op["Finish"]});
        }

        for (const auto &op : data["ms_operations"]) {
            msOperations.append({op["Job"], op["Machine"], op["Start"], op["Finish"]});
        }
    } catch (json::type_error &e) {
        std::cerr << "JSON type error: " << e.what() << std::endl;
        return 1;
    }

    // Создание виджетов
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    plot1 = new QCustomPlot;
    plot2 = new QCustomPlot;
    mainLayout->addWidget(plot1);
    mainLayout->addWidget(plot2);

    QHBoxLayout *formLayout = new QHBoxLayout;

    QVBoxLayout *labelsLayout = new QVBoxLayout;
    QLabel *startTimeLabel = new QLabel("Start Time:");
    QLabel *finishTimeLabel = new QLabel("Finish Time:");
    QLabel *jobLabel = new QLabel("Job:");
    QLabel *machineLabel = new QLabel("Machine:");
    labelsLayout->addWidget(startTimeLabel);
    labelsLayout->addWidget(finishTimeLabel);
    labelsLayout->addWidget(jobLabel);
    labelsLayout->addWidget(machineLabel);

    QVBoxLayout *inputsLayout = new QVBoxLayout;
    startTimeEdit = new QLineEdit;
    finishTimeEdit = new QLineEdit;
    jobEdit = new QLineEdit;
    machineEdit = new QLineEdit;
    inputsLayout->addWidget(startTimeEdit);
    inputsLayout->addWidget(finishTimeEdit);
    inputsLayout->addWidget(jobEdit);
    inputsLayout->addWidget(machineEdit);

    formLayout->addLayout(labelsLayout);
    formLayout->addLayout(inputsLayout);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    QPushButton *applyButton = new QPushButton("Apply");
    QPushButton *resetButton = new QPushButton("Reset");
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(resetButton);

    mainLayout->addLayout(buttonsLayout);

    window.setCentralWidget(centralWidget);

    // Установка графиков
    plotGraphs();

    QObject::connect(applyButton, &QPushButton::clicked, updateBar);
    QObject::connect(resetButton, &QPushButton::clicked, []() {
        plotGraphs();
        startTimeEdit->clear();
        finishTimeEdit->clear();
        jobEdit->clear();
        machineEdit->clear();
    });

    window.resize(800, 600);
    window.show();

    QObject::connect(plot1, &QCustomPlot::mousePress, [plot1](QMouseEvent *event) {
        onBarClicked(event, plot1);
    });

    QObject::connect(plot2, &QCustomPlot::mousePress, [plot2](QMouseEvent *event) {
        onBarClicked(event, plot2);
    });

    return a.exec();
}
