#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.LoadJsonData("../operation_data.json");
    mainWindow.show();

    return app.exec();
}

