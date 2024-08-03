#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    secondWindow = new SecondWindow(this, 1400, 800); // Задаем размеры внутреннего окна
    setCentralWidget(secondWindow);
    setMinimumSize(1800, 800); // Устанавливаем минимальный размер окна
}

MainWindow::~MainWindow()
{
    delete secondWindow;
}
