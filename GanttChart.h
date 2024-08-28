#ifndef GANTTCHART_H
#define GANTTCHART_H

#include <vector>
#include <unordered_map>
#include <QPainter>
#include <QColor>

// Определение структуры SOperation для хранения операций
struct SOperation {
    int iStart;
    int iFinish;
    int iJob;
    int iMachine;
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
    void DrawGanttChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight);

private:
    std::vector<SOperation> m_vJsOperations;
    std::vector<SOperation> m_vMsOperations;
    std::unordered_map<int, QColor> m_umapJobColors;
    std::unordered_map<int, QColor> m_umapMachineColors;

    void InitializeColors();
};

#endif // GANTTCHART_H
