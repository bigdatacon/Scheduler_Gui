
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QScrollArea>
#include "GanttChartWidget.h"
#include <QFileDialog>
#include "GlobalState.h"
#include <QCheckBox>
#include <QTimer>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow(); // Объявление деструктора
    void LoadData(const QString &filename); // Добавляем метод для загрузки JSON данных

private slots:
    void onSolveButtonClicked();
    void onShowTimeButtonClicked();
    void onShowMetricsButtonClicked();
    void onRestartSolverButtonClicked();
    void onZoomInButtonClicked();
    void onZoomOutButtonClicked();
    void onOptimizationModeChanged();
    void onZoomResetClicked();

private:
    void resizeEvent(QResizeEvent *event);

    GanttChartWidget *m_pChartWidget;
    QScrollArea *m_pScrollArea;

    // Тулбар и элементы управления
    QToolBar *m_pToolBar;
    QPushButton *m_pSolveButton;
    QPushButton *m_pShowTimeButton;
    QPushButton *m_pShowMetricsButton;
    QPushButton *m_pRestartSolverButton;
    QPushButton *m_pZoomInButton;
    QPushButton *m_pZoomOutButton;
//    QLineEdit *m_pZoomLabel;
    QPushButton* m_pZoomButton;

    // Радиокнопки для выбора режима оптимизации
    QRadioButton *m_pDurationRadioButton;
    QRadioButton *m_pCostRadioButton;
    QRadioButton *m_pSetupsRadioButton;

    QCheckBox* m_pHeuristicsButton1;  // Эвристика 1
    QCheckBox* m_pHeuristicsButton2;  // Эвристика 2
    QTimer* m_pSolverCheckTimer;  // Таймер для проверки статуса солвера
};

#endif // MAINWINDOW_H
