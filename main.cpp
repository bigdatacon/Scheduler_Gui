#include <QApplication>
#include "GanttChartMainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GanttChartMainWindow mainWindow;
    mainWindow.LoadJsonData("../operation_data.json");
    mainWindow.show();

    return app.exec();
}
