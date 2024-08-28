#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GanttChartWidget.h"


// 1. Переименовать - убрать logic из названия класса logic
// 2. ChantChartMainWindow - переименовать в MainWindow
// 3. тут GanttChartLogic m_oLogic; сделать указатель
// 4. в privat ChantChartWindget созадть тулбар - поместить 1 кнопку ("запустить солвер из файла")
// 5. опицонально задать прогресс бар от числа итераций или иначе - базово итерации задавать пользователем


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void LoadJsonData(const QString &filename) {
        m_pChartWidget->LoadJsonData(filename);
    }

private:
    GanttChartWidget *m_pChartWidget;
};

#endif // MAINWINDOW_H
