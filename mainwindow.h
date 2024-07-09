#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

class MSOperation {
public:
    int machineId;
    int operationId;
    int startTime;
    int endTime;

    MSOperation(int mId, int oId, int sTime, int eTime)
        : machineId(mId), operationId(oId), startTime(sTime), endTime(eTime) {}
};

class JSOperation {
public:
    int jobId;
    int machineId;
    int startTime;
    int endTime;

    JSOperation(int jId, int mId, int sTime, int eTime)
        : jobId(jId), machineId(mId), startTime(sTime), endTime(eTime) {}
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleBarDrag(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event);

private:
    Ui::MainWindow *ui;
    QCustomPlot *msPlot;
    QCustomPlot *jsPlot;

    std::vector<std::vector<MSOperation*>> ms_operations;
    std::vector<std::vector<JSOperation*>> js_operations;

    void setupPlots();
    void plotMSOperations();
    void plotJSOperations();
    void syncPlots(QCPBars *source, QCPBars *target);
};

#endif // MAINWINDOW_H
