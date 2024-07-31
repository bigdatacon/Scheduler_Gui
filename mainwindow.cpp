// mainwindow.cpp
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      secondWindow(new SecondWindow(this))
{
    setCentralWidget(secondWindow);
}

MainWindow::~MainWindow()
{
    delete secondWindow;
}
