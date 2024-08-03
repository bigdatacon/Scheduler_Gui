#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//    secondWindow = new SecondWindow(this, 1400, 800); // Задаем размеры внутреннего окна
    secondWindow = new SecondWindow(this); // Правильный синтаксис инициализации
    setCentralWidget(secondWindow);
    setMinimumSize(800, 600); // Устанавливаем минимальный размер окна
}

MainWindow::~MainWindow()
{
    delete secondWindow;
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    secondWindow->resize(size()); // Устанавливаем размер SecondWindow на весь размер MainWindow
}
