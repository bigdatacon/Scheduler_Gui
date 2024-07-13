#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onApplyButtonClicked();
    void onResetButtonClicked();

private:
    QCustomPlot *customPlot;
    QCPBars *machinesBars;
    QCPBars *jobsBars;
    QLineEdit *startTimeEdit;
    QLineEdit *finishTimeEdit;
    QLineEdit *jobEdit;
    QLineEdit *machineEdit;
    QPushButton *applyButton;
    QPushButton *resetButton;
    QVBoxLayout *layout;
    void setupPlot();
    void updateBars();
};

#endif // MAINWINDOW_H
