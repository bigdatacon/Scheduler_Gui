#include <QApplication>
#include "GanttChart.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Создаем график и загружаем данные из JSON
    GanttChart ganttChart;
    ganttChart.LoadJsonData("../operation_data.json");
    ganttChart.show();

    return app.exec();
}
