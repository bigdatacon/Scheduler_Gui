#include <QApplication>
#include "MainWindow.h"
#include "iostream"
#include "filesystem"
#include "QDebug"
#include <QStatusBar>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.LoadData("data_20_15_3_base.txt");
//    mainWindow.resize(VIRTUAL_SCREEN_WIDT, VIRTUAL_SCREEN_HEIGHT);

    int statusBarHeight = mainWindow.statusBar()->sizeHint().height();
    mainWindow.resize(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT + statusBarHeight);

    mainWindow.showMaximized(); // Раскрыть окно на доступную область экрана (без системных панелей)
    return app.exec();
}

