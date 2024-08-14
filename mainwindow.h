#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "secondwindow.h"
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showEvent(QShowEvent *event);

    void printRealSize();

private slots:
    void onPushButtonClicked();

private:
    SecondWindow *secondWindow;

    // Добавляем виджеты как приватные члены
    QPushButton *pushButton;
    QRadioButton *radioButton1;
    QRadioButton *radioButton2;
};

#endif // MAINWINDOW_H
