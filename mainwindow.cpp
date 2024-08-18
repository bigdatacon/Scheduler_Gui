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
    // Получаем текущую рабочую директорию
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current working directory: " << currentPath << std::endl;

    // Формируем пути для проверки
    std::filesystem::path libPathInCurrent = currentPath / "solver_program" / "build" / "libsolver.so";
    std::filesystem::path libPathInParent = currentPath.parent_path() / "solver_program" / "build" / "libsolver.so";

    std::filesystem::path libPath;

    // Проверяем, где находится библиотека
    if (std::filesystem::exists(libPathInCurrent)) {
        libPath = libPathInCurrent;
    } else if (std::filesystem::exists(libPathInParent)) {
        libPath = libPathInParent;
    } else {
        std::cerr << "Не удалось найти библиотеку libsolver.so ни в текущей директории, ни на уровне выше" << std::endl;
        return;
    }

    // Загружаем библиотеку
    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Не удалось загрузить библиотеку: " << dlerror() << std::endl;
        return;
    }

    std::cout << "Библиотека успешно загружена: " << libPath << std::endl;

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

    // Формируем пути к JSON-файлам
    std::filesystem::path inputJsonPath;
    std::filesystem::path outputJsonPath;

    if (libPath == libPathInCurrent) {
        inputJsonPath = currentPath / "solver_program" / "inputdata.json";
        outputJsonPath = currentPath / "solver_program" / "outputdata.json";
    } else {
        inputJsonPath = currentPath.parent_path() / "solver_program" / "inputdata.json";
        outputJsonPath = currentPath.parent_path() / "solver_program" / "outputdata.json";
    }

    // Читаем входные данные из JSON
    SolverData inputData = readInputFunc(inputJsonPath.string());

    // Выполняем вычисления
    result res = solveFunc(inputData);

    // Записываем результат в JSON
    writeOutputFunc(res, outputJsonPath.string());

    // Выводим результат в консоль
    qDebug() << "Solver result: y1 =" << res.y1 << ", y2 =" << res.y2;

    // Отображаем результат в QLabel
    ui->resultLabel->setText(QString("Result: y1 = %1, y2 = %2").arg(res.y1).arg(res.y2));

    // Закрываем библиотеку
    dlclose(handle);
}



