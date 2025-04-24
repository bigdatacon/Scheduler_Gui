#ifndef GANTTCHARTWIDGET_H
#define GANTTCHARTWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include "GanttChart.h"

#include <QToolBar>
#include <QPushButton>  // Добавляем этот include для кнопки
#include <QTextEdit>  // Добавляем для текстового поля
#include <QDialog>    // Для диалогового окна
#include <QRadioButton>  // Для радиокнопок
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QScrollArea>



class GanttChartWidget : public QWidget {
    Q_OBJECT

public:
//    GanttChartWidget(QWidget *pParent, GanttChart *pGanttChart,
//                     QPushButton *m_pZoomButton, int m_toolbarHeight, QScrollArea *pScrollArea,
//                     int statusBarHeight);

    GanttChartWidget(QWidget *pParent, GanttChart *pGanttChart,
                     QPushButton *m_pZoomButton, int m_toolbarHeight,
                     int statusBarHeight);


    ~GanttChartWidget();
    void LoadData(const QString &sFilename);
    void setToolbarHeight(int height);



protected:
    friend class MainWindow;
    void showEvent(QShowEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void handleRightClick(QPoint clickPos, std::vector<SJobOperation> &jsOperations, std::vector<SResourceOperation> &msOperations, std::vector<sUnavData> &vUnavData);

    void updateChart();
    void onWorkersTimeButtonClicked();

    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void UpdateSize(); // Объявляем как слот
    void setZoom(double zoomLevel) ;

private slots:
    void OnSolveButtonClicked();  // Обработчик для нажатия кнопки
    void OnShowScheduleMetricsClicked();  // Новый слот для открытия окна с данными

    void OnSolveButtonClicked_SolverRestart();// Обработчик для нажатия кнопки перезапустить solver

    void OnZoomOutClicked();  // Слот для уменьшения зума
    void OnZoomInClicked();   // Слот для увеличения зума

    void OnZoomInClickedScroll(const QPointF &mousePos);
    void OnZoomOutClickedScroll(const QPointF &mousePos);
//    QScrollArea* findScrollArea() const;

    double scrollBarValueToDouble(QScrollBar *scrollBar, double zoom) const ;

    int doubleToScrollBarValue(double value, double zoom) const ;



    // В секции signals:
signals:
    void statusTextChanged(const QString &text);

private:
    void Initialize();
    void DrawGanttChart();
    void DrawWorkersTimeChart();
    void UpdateBar(int iJob, int iMachine, int iNewStart, int iNewJob);
    void updateZoomedImages();  // Метод для обновления изображений после изменения зума
    void PerformDelayedInitialization() ;
    void drawImageWithOffset(QPainter &oPainter, const QImage &sourceImage, const QRect &visibleRect);
    void printStackTrace(const QString &message);
    void setPreciseOffset(QPointF offset, const QString &context);
    void updateScrollbars();


    QPoint clampOffsetToValidRange(const QPoint& offset) const;
    QSize getCurrentImageSize() const;




    GanttChart* m_pGanttChart;
//    QScrollArea* m_pScrollArea; // Указатель на область прокрутки

    QImage m_oChartImage;
    QImage m_oWorkersImage;

    QPoint m_qDragStartPosition;
    int m_iDraggedJob;
    int m_iDraggedMachine;
    bool m_bJsMode;

    QToolBar* m_pToolBar;  // Добавляем тулбар
    QPushButton* m_pSolveButton;  // Кнопка для запуска солвера
    QPushButton* m_pShowTimeButton; // кнопка отображения времени по рабочим

    bool m_bDisplayingWorkersTimeChart = false; // Flag to track which chart to display

    QPushButton* m_pShowMetricsButton;
    ScheduleMetrics m_ScheduleMetrics;  // Добавляем объект структуры

    QPushButton* m_pRestartSolverButton; // Кнопка для перезапуска солвера

    // Элементы управления зумом
    QPushButton* m_pZoomOutButton;  // Кнопка для уменьшения зума
    QPushButton* m_pZoomInButton;   // Кнопка для увеличения зума
    QPushButton* m_pZoomButton;
//    QLineEdit* m_pZoomLabel;  // Отображение текущего значения зума

    // Добавляем радиокнопки
    QRadioButton* m_pDurationRadioButton; // Кнопка для установки режима оптимизации длительности
    QRadioButton* m_pCostRadioButton; // Кнопка для установки режима оптимизации стоимости
    QRadioButton* m_pSetupsRadioButton; // Кнопка для установки режима оптимизации переналадок

    QCheckBox* m_pDoublePowerButton;  // Кнопка для удвоения мощности
    QCheckBox* m_pQuadruplePowerButton;  // Кнопка для учетверения мощности
    

    QString m_filename;
    bool b_yet_draw_ghant_chart = false;
    bool b_yet_draw_workers_chart = false;

//    int m_toolbarHeight = 0; // Высота тулбара
    int m_toolbarHeight ; // Высота тулбара
    int m_scrollAreaHeight;
    bool b_first_draw_workers_chart = false;

    bool m_bHandToolActive = false;  // Активна ли ладонь


    double m_dHorizontalScrollOffset = 0.0;
    double m_dVerticalScrollOffset = 0.0;

    QPoint m_offset; // Сдвиг изображения
    QPoint m_lastMousePos; // Последнее положение мыши

    bool m_bendScroll = false; // Переменная для отслеживания состояния прокрутки
    int m_istatusBarHeight;

    QPointF m_fPreciseScrollOffset = QPointF(0.0, 0.0);  // Точный offset без округления
//    QPointF m_mouseImagePosSavedBeforeZoom = QPointF(0, 0);
    bool m_bManualResizeOnly = false;

    QPointF m_mouseImagePosBeforeScroll;

    QScrollBar* m_pHScrollBar = nullptr;
    QScrollBar* m_pVScrollBar = nullptr;











};

#endif // GANTTCHARTWIDGET_H
