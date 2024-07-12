#include <QApplication>
#include <QMainWindow>
#include "qcustomplot.h"
#include <QVBoxLayout>

struct Operation {
    int Start;
    int Finish;
    int Job;
    int Machine;
};

void createGanttChart(QCustomPlot *customPlot, const QVector<Operation> &operations, bool groupByJob) {
    QVector<double> startTimes, durations;
    QVector<QString> labels;
    QMap<int, int> uniqueLabels; // To map job/machine to axis index
    int index = 0;

    for (const auto &op : operations) {
        int label = groupByJob ? op.Job : op.Machine;
        if (!uniqueLabels.contains(label)) {
            uniqueLabels[label] = index++;
        }
        startTimes.append(op.Start);
        durations.append(op.Finish - op.Start);
        labels.append(groupByJob ? QString("Job %1").arg(op.Job) : QString("Machine %1").arg(op.Machine));
    }

    QCPBars *bars = new QCPBars(customPlot->yAxis, customPlot->xAxis);
    bars->setData(startTimes, durations);

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

    // Sample data from the provided JSON
    QVector<Operation> js_operations = {
        {38, 71, 1, 3}, {71, 102, 1, 0}, {102, 129, 1, 1}, {129, 169, 1, 1}, {169, 204, 1, 0},
        {0, 14, 2, 6}, {14, 35, 2, 5}, {71, 109, 2, 3}, {109, 150, 2, 7},
        {0, 38, 3, 3}, {38, 65, 3, 5}, {65, 107, 3, 4}, {109, 173, 3, 3}, {173, 227, 3, 7}, {227, 254, 3, 5}
    };

    QVector<Operation> ms_operations = {
        {71, 102, 0, 1}, {169, 204, 0, 1}, {102, 129, 0, 2}, {129, 169, 0, 2}, {0, 38, 2, 3},
        {38, 71, 0, 3}, {71, 109, 1, 3}, {109, 173, 2, 3}, {65, 107, 2, 4}, {14, 35, 1, 5},
        {38, 65, 2, 5}, {227, 254, 2, 5}, {0, 14, 1, 6}, {109, 150, 1, 7}, {173, 227, 2, 7}
    };

    QWidget *centralWidget = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    window.setCentralWidget(centralWidget);

    QCustomPlot *customPlot1 = new QCustomPlot();
    QCustomPlot *customPlot2 = new QCustomPlot();

    layout->addWidget(customPlot1);
    layout->addWidget(customPlot2);

    createGanttChart(customPlot1, js_operations, true);
    createGanttChart(customPlot2, ms_operations, false);

    window.resize(800, 600);
    window.show();

    return app.exec();
}
