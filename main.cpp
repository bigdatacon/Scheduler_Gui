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

using json = nlohmann::json;

struct Operation {
    int job;
    int machine;
    int start;
    int finish;
};

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
        textTicker->addTick(it.value(), QString("J%1").arg(it.key()));
    }

    customPlot->yAxis->setTicker(textTicker);
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel(groupByJob ? "Jobs" : "Machines");
    customPlot->xAxis->setRange(0, 300);
    customPlot->yAxis->setRange(-1, index);
    customPlot->replot();
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow window;

    // Загрузка данных из JSON файла
    std::ifstream file("data.json");
    json data;
    file >> data;
    file.close();

    QVector<Operation> jsOperations;
    QVector<Operation> msOperations;

    for (const auto &op : data["js_operations"]) {
        jsOperations.append({op["Job"], op["Machine"], op["Start"], op["Finish"]});
    }

    for (const auto &op : data["ms_operations"]) {
        msOperations.append({op["Job"], op["Machine"], op["Start"], op["Finish"]});
    }

    // Создание виджетов
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QCustomPlot *plot1 = new QCustomPlot;
    QCustomPlot *plot2 = new QCustomPlot;
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
    QLineEdit *startTimeEdit = new QLineEdit;
    QLineEdit *finishTimeEdit = new QLineEdit;
    QLineEdit *jobEdit = new QLineEdit;
    QLineEdit *machineEdit = new QLineEdit;
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
    setupPlot(plot1, jsOperations, false);
    setupPlot(plot2, jsOperations, true);

    window.resize(800, 600);
    window.show();

    return a.exec();
}
