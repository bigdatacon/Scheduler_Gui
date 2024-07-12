#include <QApplication>
#include <QMainWindow>
#include "qcustomplot.h"
#include <QVBoxLayout>

struct MSOperation {
    int jobIndex;
    int operationIndex;
    int startTime;
    int finishTime;
};

struct JSOperation {
    int operationIndex;
    int machineIndex;
    int startTime;
    int finishTime;
};

void createGanttChart(QCustomPlot *customPlot, const QVector<JSOperation> &js_operations, const QVector<MSOperation> &ms_operations, bool groupByJob) {
    QVector<double> startTimes, durations;
    QVector<QString> labels;
    QMap<int, int> uniqueLabels; // To map job/machine to axis index
    int index = 0;

    if (groupByJob) {
        for (const auto &op : js_operations) {
            int label = op.operationIndex;
            if (!uniqueLabels.contains(label)) {
                uniqueLabels[label] = index++;
            }
            startTimes.append(uniqueLabels[label]);
            durations.append(op.finishTime - op.startTime);
            labels.append(QString("M%1").arg(op.machineIndex));
        }
    } else {
        for (const auto &op : ms_operations) {
            int label = op.jobIndex;
            if (!uniqueLabels.contains(label)) {
                uniqueLabels[label] = index++;
            }
            startTimes.append(uniqueLabels[label]);
            durations.append(op.finishTime - op.startTime);
            labels.append(QString("J%1").arg(op.jobIndex));
        }
    }

    QCPBars *bars = new QCPBars(customPlot->yAxis, customPlot->xAxis);
    QVector<double> barWidths(startTimes.size(), 1); // Width of each bar
    bars->setData(startTimes, durations);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (auto it = uniqueLabels.begin(); it != uniqueLabels.end(); ++it) {
        textTicker->addTick(it.value(), labels[it.key()]);
    }

    customPlot->yAxis->setTicker(textTicker);
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel(groupByJob ? "Operations" : "Jobs");
    customPlot->xAxis->setRange(0, 260);
    customPlot->yAxis->setRange(-1, index);

    customPlot->replot();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow window;

    // Sample data from the provided JSON
    QVector<JSOperation> js_operations = {
        {1, 3, 38, 71}, {1, 0, 71, 102}, {1, 1, 102, 129}, {1, 1, 129, 169}, {1, 0, 169, 204},
        {2, 6, 0, 14}, {2, 5, 14, 35}, {2, 3, 71, 109}, {2, 7, 109, 150},
        {3, 3, 0, 38}, {3, 5, 38, 65}, {3, 4, 65, 107}, {3, 3, 109, 173}, {3, 7, 173, 227}, {3, 5, 227, 254}
    };

    QVector<MSOperation> ms_operations = {
        {0, 1, 71, 102}, {0, 1, 169, 204}, {0, 2, 102, 129}, {0, 2, 129, 169}, {2, 3, 0, 38},
        {0, 3, 38, 71}, {1, 3, 71, 109}, {2, 3, 109, 173}, {2, 4, 65, 107}, {1, 5, 14, 35},
        {2, 5, 38, 65}, {2, 5, 227, 254}, {1, 6, 0, 14}, {1, 7, 109, 150}, {2, 7, 173, 227}
    };

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
