#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secondwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QDebug>
#include "solver.h"
#include <dlfcn.h>  // Для динамической загрузки библиотеки в Linux
#include <filesystem>
#include <iostream>


// Типы функций, которые мы будем загружать из DLL
typedef result (*SolveFunc)(const SolverData&);
typedef SolverData (*ReadInputFunc)(const std::string&);
typedef void (*WriteOutputFunc)(const result&, const std::string&);


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
    // Загружаем динамическую библиотеку
//    void* handle = dlopen("/home/user1/QT_Projects/gch_2/solver_program/build/libsolver.so", RTLD_LAZY);

    // Получаем текущую рабочую директорию
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current working directory: " << currentPath << std::endl;

    // Поднимаемся на одну директорию выше
    std::filesystem::path projectRoot = currentPath.parent_path();

    // Формируем путь к библиотеке libsolver.so
    std::filesystem::path libPath = projectRoot / "solver_program" / "build" / "libsolver.so";

    // Загружаем библиотеку
    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Не удалось загрузить библиотеку: " << dlerror() << std::endl;
        return;
    }

    // Если библиотека успешно загружена, продолжаем работу
    std::cout << "Библиотека успешно загружена: " << libPath << std::endl;


    if (!handle) {
        qDebug() << "Не удалось загрузить библиотеку:" << dlerror();
        return;
    }

    // Очищаем ошибки
    dlerror();

    // Загружаем функции
    SolveFunc solveFunc = (SolveFunc)dlsym(handle, "solve");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        qDebug() << "Не удалось загрузить функцию solve:" << dlsym_error;
        dlclose(handle);
        return;
    }

    ReadInputFunc readInputFunc = (ReadInputFunc)dlsym(handle, "readInputData");
    dlsym_error = dlerror();
    if (dlsym_error) {
        qDebug() << "Не удалось загрузить функцию readInputData:" << dlsym_error;
        dlclose(handle);
        return;
    }

    WriteOutputFunc writeOutputFunc = (WriteOutputFunc)dlsym(handle, "writeOutputData");
    dlsym_error = dlerror();
    if (dlsym_error) {
        qDebug() << "Не удалось загрузить функцию writeOutputData:" << dlsym_error;
        dlclose(handle);
        return;
    }

    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    // Читаем входные данные из JSON
//    SolverData inputData = readInputFunc("solver_program/inputdata.json");
    SolverData inputData = readInputFunc("../solver_program/inputdata.json");

    // Выполняем вычисления
    result res = solveFunc(inputData);

    // Записываем результат в JSON
//    writeOutputFunc(res, "solver_program/outputdata.json");
    writeOutputFunc(res, "../solver_program/outputdata.json");;

    // Выводим результат в консоль
        qDebug() << "Solver result: y1 =" << res.y1 << ", y2 =" << res.y2;

    // Отображаем результат в QLabel
    ui->resultLabel->setText(QString("Result: y1 = %1, y2 = %2").arg(res.y1).arg(res.y2));

    // Закрываем библиотеку
    dlclose(handle);
}


//void MainWindow::onPushButtonClicked()
//{
//    qDebug() << "Button 1 clicked!";

//    // Создаем объект Solver
//    Solver solver;

//    // Задаем данные
//    SolverData inputData;
//    inputData.x1 = 1;
//    inputData.x2 = 2;
//    inputData.x3 = 3;

//    // Выполняем вычисление
//    result res = solver.solve(inputData);

//    // Выводим результат в консоль
//    qDebug() << "Solver result: y1 =" << res.y1 << ", y2 =" << res.y2;

//    // Выводим результат в QLabel
//    ui->resultLabel->setText(QString("Result: y1 = %1, y2 = %2").arg(res.y1).arg(res.y2));
//}
