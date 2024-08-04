#include "mainwindow.h"
#include "secondwindow.h"
#include <QScrollArea>

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
}

MainWindow::~MainWindow()
{
    delete secondWindow;
}
