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
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }

    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);

    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");

    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(-1, 1);
    customPlot->yAxis->setRange(0, 1);

    customPlot->replot();

    window.show();
    return app.exec();
}
