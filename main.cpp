#include <QApplication>
#include "MainWindow.h"
#include "iostream"
#include "filesystem"
#include <dlfcn.h>  // Для динамической загрузки библиотеки в Linux
//#include "Solver.h" // Подключаем минимальный заголовочный файл

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.LoadJsonData("../operation_data.json");
    mainWindow.show();


//// Получаем текущую рабочую директорию
//    std::filesystem::path currentPath = std::filesystem::current_path();
//    std::cout << "Current working directory: " << currentPath << std::endl;

//    // Формируем пути для проверки
////    std::filesystem::path libPathInCurrent = currentPath / "build_dylib" / "libsolver.dylib";
////    std::filesystem::path libPathInParent = currentPath.parent_path() / "Scheduling-С++" / "build_dylib" / "libsolver.dylib";
//    std::filesystem::path libPathInCurrent = currentPath / "libsolver.so";
//    std::filesystem::path libPathInParent = currentPath.parent_path() / "libsolver.so";

//    std::filesystem::path libPath;

//    // Проверяем, где находится библиотека
//    if (std::filesystem::exists(libPathInCurrent)) {
//        libPath = libPathInCurrent;
//    } else if (std::filesystem::exists(libPathInParent)) {
//        libPath = libPathInParent;
//    } else {
//        std::cerr << "Не удалось найти библиотеку libsolver.dylib ни в текущей директории, ни на уровне выше" << std::endl;
//        return 0;
//    }

//    // Загружаем библиотеку
//    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
//    if (!handle) {
//        std::cerr << "Не удалось загрузить библиотеку: " << dlerror() << std::endl;
//        return 0;
//    }

//    std::cout << "Библиотека успешно загружена: " << libPath << std::endl;

//    // Сбрасываем предыдущие ошибки
//    dlerror();

//    // Получаем указатели на функции
//    typedef Solver* (*CreateSolverFunc)();
//    typedef void (*DestroySolverFunc)(Solver*);
//    typedef void (Solver::*RunFunc)();

//    CreateSolverFunc createSolver = (CreateSolverFunc)dlsym(handle, "CreateSolver");
//    DestroySolverFunc destroySolver = (DestroySolverFunc)dlsym(handle, "DestroySolver");

//    const char* dlsym_error = dlerror();
//    if (dlsym_error) {
//        std::cerr << "Не удалось получить функцию: " << dlsym_error << std::endl;
//        dlclose(handle);
//        return 1;
//    }

//    // Создаем объект
//    Solver* pSolver = createSolver();
//    if ( !pSolver )
//    {
//        std::cerr << "Не удалось создать объект Solver" << std::endl;
//        dlclose(handle);
//        return 1;
//    }

//    // Используем объект
//    pSolver->SetIterationsCount(1);
//    pSolver->LoadProblemFromFileJSON("data8_3_15.json");
//    pSolver->Run();
//    MSchedule* pMSchedule = pSolver->GetMSchedule();

//    // Уничтожаем объект
//    destroySolver(pSolver);

//    // Закрываем библиотеку
//    dlclose(handle);


    return app.exec();
}

