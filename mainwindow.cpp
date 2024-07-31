#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    secondWindow = new SecondWindow(this, 800, 600); // Задаем размеры внутреннего окна
    setCentralWidget(secondWindow);
    setMinimumSize(800, 600); // Устанавливаем минимальный размер окна
}

MainWindow::~MainWindow()
{
    delete secondWindow;
}
