#include "mainwindow.h"
#include "secondwindow.h"
#include <QScrollArea>
#include <iostream>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    secondWindow = new SecondWindow(this, 1400, 800); // Задаем размеры внутреннего окна
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(secondWindow);
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);

//    setCentralWidget(secondWindow);
    setMinimumSize(800, 600); // Устанавливаем минимальный размер окна

//    auto main_window_width = this->width();
//    auto main_window_height = this->height();
//    int a = 10;
//    std::cout <<a;
}

MainWindow::~MainWindow()
{
    delete secondWindow;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    QTimer::singleShot(0, this, &MainWindow::printRealSize);
}

void MainWindow::printRealSize()
{
    int realWidth = this->width();
    int realHeight = this->height();
    std::cout << std::endl;
    std::cout << "Real MainWindow size : " << realWidth << "x" << realHeight << std::endl;
}
