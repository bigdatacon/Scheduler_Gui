#include <QApplication>
#include <QMainWindow>
#include "qcustomplot.h"
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct MSOperation {
    int job;
    int machine;
    int startTime;
    int finishTime;
};

struct JSOperation {
    int job;
    int machine;
    int startTime;
    int finishTime;
};

bool loadJsonData(const QString &fileName, QVector<JSOperation> &js_operations, QVector<MSOperation> &ms_operations) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(data);
    if (document.isNull() || !document.isObject()) {
        return false;
    }

    QJsonObject jsonObject = document.object();
    QJsonArray jsArray = jsonObject["js_operations"].toArray();
    QJsonArray msArray = jsonObject["ms_operations"].toArray();

    for (const QJsonValue &value : jsArray) {
        QJsonObject obj = value.toObject();
        JSOperation operation;
        operation.job = obj["Job"].toInt();
        operation.machine = obj["Machine"].toInt();
        operation.startTime = obj["Start"].toInt();
        operation.finishTime = obj["Finish"].toInt();
        js_operations.append(operation);
    }

    for (const QJsonValue &value : msArray) {
        QJsonObject obj = value.toObject();
        MSOperation operation;
        operation.job = obj["Job"].toInt();
        operation.machine = obj["Machine"].toInt();
        operation.startTime = obj["Start"].toInt();
        operation.finishTime = obj["Finish"].toInt();
        ms_operations.append(operation);
    }

    return true;
}

void createGanttChart(QCustomPlot *customPlot, const QVector<JSOperation> &js_operations, const QVector<MSOperation> &ms_operations, bool groupByJob) {
    QVector<double> startTimes, durations, yPositions;
    QVector<QString> labels;
    QMap<int, int> uniqueLabels; // To map job/machine to axis index
    int index = 0;

    if (groupByJob) {
        for (const auto &op : js_operations) {
            int label = op.job;
            if (!uniqueLabels.contains(label)) {
                uniqueLabels[label] = index++;
            }
            startTimes.append(op.startTime);
            durations.append(op.finishTime - op.startTime);
            yPositions.append(uniqueLabels[label]);
            labels.append(QString("J%1").arg(op.job));
        }
    } else {
        for (const auto &op : ms_operations) {
            int label = op.machine;
            if (!uniqueLabels.contains(label)) {
                uniqueLabels[label] = index++;
            }
            startTimes.append(op.startTime);
            durations.append(op.finishTime - op.startTime);
            yPositions.append(uniqueLabels[label]);
            labels.append(QString("M%1").arg(op.machine));
        }
    }

    QCPBars *bars = new QCPBars(customPlot->yAxis, customPlot->xAxis);
    bars->setData(yPositions, durations);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (auto it = uniqueLabels.begin(); it != uniqueLabels.end(); ++it) {
        textTicker->addTick(it.value(), labels[it.key()]);
    }

    customPlot->yAxis->setTicker(textTicker);
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel(groupByJob ? "Jobs" : "Machines");
    customPlot->xAxis->setRange(0, 260);
    customPlot->yAxis->setRange(-1, index);

    customPlot->replot();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow window;

    QVector<JSOperation> js_operations;
    QVector<MSOperation> ms_operations;
    if (!loadJsonData("data.json", js_operations, ms_operations)) {
        qDebug() << "Failed to load data from JSON file.";
        return -1;
    }

    QWidget *centralWidget = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    window.setCentralWidget(centralWidget);

    QCustomPlot *customPlot1 = new QCustomPlot();
    QCustomPlot *customPlot2 = new QCustomPlot();

    layout->addWidget(customPlot1);
    layout->addWidget(customPlot2);

    createGanttChart(customPlot1, js_operations, ms_operations, true);
    createGanttChart(customPlot2, js_operations, ms_operations, false);

    window.resize(800, 600);
    window.show();

    return app.exec();
}
