#ifndef GANTTCHART_H
#define GANTTCHART_H

#include <vector>
#include <unordered_map>
#include <QPainter>
#include <QColor>

// Определение структуры SOperation для хранения операций
//struct SOperation {
//    int iStart;
//    int iFinish;
//    int iJob;
//    int iMachine;
//};

//struct SJobOperation {
//    int iStart;
//    int iFinish;
//    int iJob;
//    int iMachine;
//};

struct SJobOperation {
    int iStart;
    int iFinish;
    int iJob;
    std::vector<int> vMachinesIndexes;
    QRect rect; // Координаты прямоугольника для отрисовки
    bool bHighlighted = false; // Состояние выделения
};

struct SMachineOperation {
    int iStart;
    int iFinish;
    int iJob;
    int iMachine;
    QRect rect; // Координаты прямоугольника для отрисовки
    bool bHighlighted = false; // Состояние выделения
};


struct ScheduleMetrics
{
    int iScheduleTime;
    int iScheduleCost;
    int iSumDeviation;
    int iDeltaUtilization;
    int iFreeMachinesCount;
};

// Определение структуры Bar для хранения прямоугольников баров
struct Bar {
    int iJob;
    int iMachine;
    int iStart;
    int iFinish;
    QRect rect; // Координаты прямоугольника для отрисовки
};

class GanttChart {
public:
    void LoadJsonData(const QString &sFilename);
    void LoadJsonData_2(const QString &sFilename);
    std::tuple<int, int, int> calculateMaxValues();
    void DrawGanttChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight);
    void DrawWorkersTimeChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight);

    std::vector<SJobOperation>& getJsOperations() { return m_vJsOperations_cont; }
    std::vector<SMachineOperation>& getMsOperations() { return m_vMsOperations_cont; }


private:
//    std::vector<SOperation> m_vJsOperations;
//    std::vector<SOperation> m_vMsOperations;

    std::vector<SJobOperation> m_vJsOperations_cont;
    std::vector<SMachineOperation> m_vMsOperations_cont;
    std::unordered_map<int, QColor> m_umapJobColors;
    ScheduleMetrics m_ScheduleMetrics;
//    std::unordered_map<int, QColor> m_umapMachineColors;

//    std::unordered_map<Bar, Bar> matching_up_down;
//    std::unordered_map<Bar, Bar> matching_down_up;

    void InitializeColors();
};

#endif // GANTTCHART_H
