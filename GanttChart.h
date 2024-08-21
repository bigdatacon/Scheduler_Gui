#ifndef GANTTCHART_H
#define GANTTCHART_H

#include <QWidget>
#include <QImage>
#include <unordered_map>
#include <vector>

struct SOperation {
    int iStart;
    int iFinish;
    int iJob;
    int iMachine;
};

class GanttChart : public QWidget {
    Q_OBJECT

public:
    GanttChart(QWidget *pParent = nullptr);
    void LoadJsonData(const QString &sFilename);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void Initialize();
    void DrawGanttChart();
    void DrawAxesAndLabels(QPainter &oPainter, int iOffsetYJs, int iOffsetYMs, int iScaleFactorX, int iLabelOffsetX, int maxFinish);
//    void resizeEvent(QResizeEvent *event);
    void UpdateBar(int iJob, int iMachine, int iNewStart, int iNewJob);

    QImage m_oChartImage;
    std::vector<SOperation> m_vJsOperations;
    std::vector<SOperation> m_vMsOperations;
    std::unordered_map<int, QColor> m_umapJobColors;
    std::unordered_map<int, QColor> m_umapMachineColors;

    int m_iDraggedJob;
    int m_iDraggedMachine;
    QPoint m_qDragStartPosition;
    bool m_bJsMode;  // Определяет, какой график мы редактируем (js_operations или ms_operations)
};

#endif // GANTTCHART_H
