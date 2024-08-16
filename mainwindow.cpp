#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secondwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QDebug>
#include "solver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) // Инициализация объекта ui
{
    // Инициализируем виджеты из mainwindow.ui
    ui->setupUi(this);

    // Создаем контейнеры для SecondWindow и кнопок
    QVBoxLayout *centralLayout = new QVBoxLayout();  // Главный макет для centralwidget

    // Создаем виджет SecondWindow и добавляем его в главный макет
    SecondWindow *secondWindow = new SecondWindow(this, 800, 600);
    centralLayout->addWidget(secondWindow);  // Добавляем SecondWindow первым

    // Добавляем кнопки из Qt Designer
    centralLayout->addWidget(ui->pushButton);
    centralLayout->addWidget(ui->radioButton);
    centralLayout->addWidget(ui->radioButton_2);
    centralLayout->addWidget(ui->resultLabel);

    // Устанавливаем макет для центрального виджета
    ui->centralwidget->setLayout(centralLayout);

    // Поднимаем кнопки на передний план
    ui->pushButton->raise();
    ui->radioButton->raise();
    ui->radioButton_2->raise();
    ui->resultLabel->raise();

    // Подключаем сигнал clicked к слоту onPushButtonClicked
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);  // Вызов базового метода
    // Ваша реализация
}

void MainWindow::onPushButtonClicked()
{
    qDebug() << "Button 1 clicked!";

    // Создаем объект Solver
    Solver solver;

    // Задаем данные
    SolverData inputData;
    inputData.x1 = 1;
    inputData.x2 = 2;
    inputData.x3 = 3;

    // Выполняем вычисление
    result res = solver.solve(inputData);

    // Выводим результат в консоль
    qDebug() << "Solver result: y1 =" << res.y1 << ", y2 =" << res.y2;

    // Выводим результат в QLabel
    ui->resultLabel->setText(QString("Result: y1 = %1, y2 = %2").arg(res.y1).arg(res.y2));
}
