#include "mainwindow.h"
#include "secondwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Создаем виджет SecondWindow
    SecondWindow *secondWindow = new SecondWindow(this, 800, 600);

    // Создаем контейнер для кнопок
    QWidget *buttonsWidget = new QWidget(this);
    QPushButton *pushButton1 = new QPushButton("Push Button 1", buttonsWidget);
//    QPushButton *pushButton2 = new QPushButton("Button 2", buttonsWidget);
    QRadioButton *radioButton2 = new QRadioButton("Radio Button 2", buttonsWidget);

    // Устанавливаем макет для кнопок
    QVBoxLayout *buttonsLayout = new QVBoxLayout(buttonsWidget);
    buttonsLayout->addWidget(pushButton1);
//    buttonsLayout->addWidget(pushButton2);
    buttonsLayout->addWidget(radioButton2);
    buttonsWidget->setLayout(buttonsLayout);

    // Добавляем виджет SecondWindow
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(secondWindow);
    layout->addWidget(buttonsWidget);

    centralWidget->setLayout(layout);

    // Поднимаем кнопки на передний план
    buttonsWidget->raise();
    // Подключаем сигнал clicked к слоту onPushButtonClicked
    connect(pushButton1, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
}

MainWindow::~MainWindow()
{
}
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);  // Вызов базового метода
    // Ваша реализация
}
void MainWindow::onPushButtonClicked()
{
    qDebug() << "Button 1 clicked!";
    // Добавьте сюда код, который должен выполняться при нажатии кнопки
}
