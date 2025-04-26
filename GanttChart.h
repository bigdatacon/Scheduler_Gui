#ifndef GANTTCHART_H
#define GANTTCHART_H

#include <vector>
#include <unordered_map>
#include <QPainter>
#include <QColor>
#include "Solver.h"
#include <QTime>
#include <filesystem>


// Определение структуры SOperation для хранения операций
struct SJobOperation {
    int iStart;
    int iFinish;
    int iJob;
    int iOperation;
    std::vector<int> vMachinesIndexes;
    QRect rect; // Координаты прямоугольника для отрисовки
    bool bHighlighted = false; // Состояние выделения
};

struct SResourceOperation {
    int iStart;
    int iFinish;
    int iSetupTime;
    int iJob;
    int iOperation;
    bool bGroup;
    int iMachine;
    QRect rect; // Координаты прямоугольника для отрисовки
    bool bHighlighted = false; // Состояние выделения
};


// Определение структуры Bar для хранения прямоугольников баров
struct Bar {
    int iJob;
    int iMachine;
    int iStart;
    int iFinish;
    QRect rect; // Координаты прямоугольника для отрисовки
};

//определение структуры для периодов недоступности
struct sUnavData {
    int resource_num;       // Номер ресурса
    int time_start;         // Время начала периода
    int time_end;           // Время конца периода
    QRect rect;             // Координаты прямоугольника для отрисовки
};

constexpr int VIRTUAL_SCREEN_WIDTH = 1920;
constexpr int VIRTUAL_SCREEN_HEIGHT = 1080;

//constexpr int VIRTUAL_SCREEN_WIDTH = 1424;
//constexpr int VIRTUAL_SCREEN_HEIGHT = 968;

class GanttChart {
public:
    GanttChart();
//    GanttChart() : m_iZoom(1) {}  // Инициализация зума
    ~GanttChart();

    void LoadData(const QString &sFilename);
    std::tuple<int, int, int> calculateMaxValues();
    void DrawGanttChart(QPainter *pPainter, int VIRTUAL_SCREEN_WIDTH, int VIRTUAL_SCREEN_HEIGHT);
    void DrawWorkersTimeChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight);

//    void set_zoom(int zoom) { m_iZoom = zoom; }
//    int get_zoom() const { return m_iZoom; }

    void set_zoom(double zoom) { m_iZoom = zoom; }
    double get_zoom() const { return m_iZoom; }

    std::vector<SJobOperation>& getJsOperations() { return m_vJsOperations_cont; }
    std::vector<SResourceOperation>& getMsOperations() { return m_vMsOperations_cont; }
    std::vector<sUnavData>& getUnavData() { return vUnavData; }

    std::vector<SResourceOperation*> m_vMainOperations;
    std::vector<std::vector<SResourceOperation*>> m_vGroupedPredecessors;

private:
    std::vector<SJobOperation> m_vJsOperations_cont;
    std::vector<SResourceOperation> m_vMsOperations_cont;
    std::unordered_map<int, QColor> m_umapJobColors;
    ScheduleMetrics m_ScheduleMetrics;
    Solver* m_pSolver;
    vector<sUnavData> vUnavData;
    vector<Resource*> m_vResources;
    vector<Operation*> m_vOperation;

    void InitializeColors();
//    int m_iZoom;  // Переменная для зума
    double m_iZoom;  // Переменная для зума




};

#endif // GANTTCHART_H
