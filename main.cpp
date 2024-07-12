#include <QApplication>
#include <QMainWindow>
#include "qcustomplot.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow window;

    QCustomPlot *customPlot = new QCustomPlot(&window);
    window.setCentralWidget(customPlot);
    window.resize(800, 600);

    // generate some data:
    QVector<double> ticks(5), barData(5);
    for (int i = 0; i < 5; ++i)
    {
        ticks[i] = i + 1; // category values
        barData[i] = (i + 1) * 2; // bar lengths
    }

    // create and set up bars:
    QCPBars *bars = new QCPBars(customPlot->yAxis, customPlot->xAxis);
    bars->setData(ticks, barData);

    // give the axes some labels:
    customPlot->xAxis->setLabel("Value");
    customPlot->yAxis->setLabel("Category");

    // prepare y axis with category labels:
    QVector<QString> labels;
    labels << "A" << "B" << "C" << "D" << "E";
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    customPlot->yAxis->setTicker(textTicker);

    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, 10);
    customPlot->yAxis->setRange(0, 6);

    customPlot->replot();

    window.show();
    return app.exec();
}
