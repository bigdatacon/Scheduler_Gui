
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QDebug>
#include <QMessageBox>
#include "GanttChart.h"
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // В конструкторе MainWindow после создания центрального виджета:
    setStatusBar(new QStatusBar(this));
    int statusBarHeight = statusBar()->sizeHint().height();

    // Создаем область прокрутки
    m_pScrollArea = new QScrollArea(this);
    // Создаем логический объект GanttChart (локально в MainWindow)
    GanttChart *pGanttChart = new GanttChart();

    // Инициализация m_pZoomLabel в MainWindow
    m_pZoomLabel = new QLineEdit("Zoom: 1", this);
    m_pZoomLabel->setReadOnly(true);
    m_pZoomLabel->setAlignment(Qt::AlignCenter);
    m_pZoomLabel->setMaximumWidth(90);

    // Создаём тулбар
    m_pToolBar = new QToolBar(this);
    m_pToolBar->setMovable(false);
    int m_toolbarHeight = m_pToolBar->height();

    // Передаем этот объект в GanttChartWidget
    m_pChartWidget = new GanttChartWidget(this, pGanttChart, m_pZoomLabel, m_toolbarHeight, m_pScrollArea, statusBarHeight);


    // Настраиваем область прокрутки
    m_pScrollArea->setWidget(m_pChartWidget);
    m_pScrollArea->setWidgetResizable(false);

    // Создаём кнопки
    m_pSolveButton = new QPushButton(QIcon(":/resources/rocket-2.png"), "", this);
    m_pSolveButton->setToolTip("Запустить солвер из файла");

    m_pShowTimeButton = new QPushButton(QIcon(":/resources/metrics.png"), "", this);
    m_pShowTimeButton->setToolTip("Расписание / Утилизация ресурсов");

    m_pShowMetricsButton = new QPushButton(QIcon(":/resources/data.png"), "", this);
    m_pShowMetricsButton->setToolTip("Метрики расписания");

    m_pRestartSolverButton = new QPushButton(QIcon(":/resources/restart.png"), "", this);
    m_pRestartSolverButton->setToolTip("Перезапустить солвер");

    m_pZoomInButton = new QPushButton("+x", this);
    m_pZoomOutButton = new QPushButton("-x", this);


    // Радиокнопки для режима оптимизации
    m_pDurationRadioButton = new QRadioButton("Оптимизация длительности", this);
    m_pCostRadioButton = new QRadioButton("Оптимизация стоимости", this);
    m_pSetupsRadioButton = new QRadioButton("Оптимизация переналадок", this);
    m_pDurationRadioButton->setChecked(true);

    QButtonGroup *radioGroup = new QButtonGroup(this);
    radioGroup->addButton(m_pDurationRadioButton);
    radioGroup->addButton(m_pCostRadioButton);
    radioGroup->addButton(m_pSetupsRadioButton);

    // отдельно блок для эвристик
    m_pHeuristicsButton1 = new QCheckBox("Эвристика 1", this);
    m_pHeuristicsButton1->setToolTip("Эвристика выбора ресурсов");  // Всплывающая подсказка
    m_pHeuristicsButton2 = new QCheckBox("Эвристика 2", this);
    m_pHeuristicsButton2->setToolTip("Эвристика выбора работ");  // Всплывающая подсказка

    // Подключаем сигналы радиокнопок к обработчикам
    connect(m_pDurationRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            g_currentState = DURATION;
            g_iSolverPower = 0;
            qDebug() << "Режим установлен: Длительность";
        }
    });

    // Подключаем сигналы радиокнопок к обработчикам
    connect(m_pCostRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            g_currentState = COST;
            g_iSolverPower = 0;
            qDebug() << "Режим установлен: Стоимость";
        }
    });

    connect(m_pSetupsRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            g_currentState = SETUPS;
            g_iSolverPower = 0;
            qDebug() << "Режим установлен: Переналадки";
        }
    });

    // Обработчики состояния чекбоксов
    connect(m_pHeuristicsButton1, &QCheckBox::stateChanged, this, [this](int state) {
        if ( state == Qt::Checked ) {
            g_iR1 = 1;
            g_iSolverPower = 0;
            //QMessageBox::information(this, "Эвристика 1", "Эвристика выбора ресурсов включена");
        } else if ( state == Qt::Unchecked ) {
            g_iR1 = 0;
            g_iSolverPower = 0;
            //QMessageBox::information(this, "Эвристика 1", "Эвристика выбора ресурсов выключена");
        }
    });

    connect(m_pHeuristicsButton2, &QCheckBox::stateChanged, this, [this](int state) {
        if ( state == Qt::Checked ) {
            g_iJ3 = 1;
            g_iSolverPower = 0;
            //QMessageBox::information(this, "Эвристика 2", "Эвристика выбора работ включена");
        } else if ( state == Qt::Unchecked ) {
            g_iJ3 = 0;
            g_iSolverPower = 0;
            //QMessageBox::information(this, "Эвристика 2", "Эвристика выбора работ выключена");
        }
    });



    // Создаём таймер
    m_pSolverCheckTimer = new QTimer(this);

    // Подключаем таймер к слоту проверки статуса
    connect(m_pSolverCheckTimer, &QTimer::timeout, this, [this]() {
        if (g_bSolverRunning) {
            m_pRestartSolverButton->setEnabled(false);
        } else {
            m_pRestartSolverButton->setEnabled(true);
        }
    });

    // Запускаем таймер с интервалом 100 мс
    m_pSolverCheckTimer->start(100);


    // Добавляем элементы на тулбар
    m_pToolBar->addWidget(m_pSolveButton);
    m_pToolBar->addWidget(m_pShowTimeButton);
    m_pToolBar->addWidget(m_pShowMetricsButton);
    m_pToolBar->addWidget(m_pRestartSolverButton);
    m_pToolBar->addWidget(m_pZoomOutButton);
    m_pToolBar->addWidget(m_pZoomLabel);
    m_pToolBar->addWidget(m_pZoomInButton);
    m_pToolBar->addWidget(m_pDurationRadioButton);
    m_pToolBar->addWidget(m_pCostRadioButton);
    m_pToolBar->addWidget(m_pSetupsRadioButton);

    m_pToolBar->addWidget(m_pHeuristicsButton1);
    m_pToolBar->addWidget(m_pHeuristicsButton2);

    // Размещаем тулбар и диаграмму в вертикальном layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    // Устанавливаем отступы: снизу отступ равен высоте статусной строки,
    // чтобы график не заходил под неё.
    mainLayout->setContentsMargins(0, 0, 0, statusBar()->sizeHint().height());

    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_pToolBar);
    mainLayout->addWidget(m_pScrollArea);

    // Подключаем сигналы к слотам
    connect(m_pSolveButton, &QPushButton::clicked, this, &MainWindow::onSolveButtonClicked);
    connect(m_pShowTimeButton, &QPushButton::clicked, this, &MainWindow::onShowTimeButtonClicked);
    connect(m_pShowMetricsButton, &QPushButton::clicked, this, &MainWindow::onShowMetricsButtonClicked);
    connect(m_pRestartSolverButton, &QPushButton::clicked, this, &MainWindow::onRestartSolverButtonClicked);
    connect(m_pZoomInButton, &QPushButton::clicked, this, &MainWindow::onZoomInButtonClicked);
    connect(m_pZoomOutButton, &QPushButton::clicked, this, &MainWindow::onZoomOutButtonClicked);

    connect(radioGroup, &QButtonGroup::idClicked, this, &MainWindow::onOptimizationModeChanged);

    connect(m_pChartWidget, &GanttChartWidget::statusTextChanged,
            this, [this](const QString &text){
                statusBar()->showMessage(text);
            });


}

MainWindow::~MainWindow() {
    delete m_pChartWidget;  // Если указатель не управляется умным указателем
    delete m_pScrollArea;
}


void MainWindow::LoadData(const QString &filename) {
    m_pChartWidget->LoadData(filename); // Перенаправляем вызов в GanttChartWidget
}

void MainWindow::onSolveButtonClicked() {
    m_pChartWidget->OnSolveButtonClicked();
}


void MainWindow::onShowTimeButtonClicked() {
    // Переключение между графиками
    m_pChartWidget->onWorkersTimeButtonClicked();
}

void MainWindow::onShowMetricsButtonClicked() {
    // Отображение метрик
    m_pChartWidget->OnShowScheduleMetricsClicked();
}

void MainWindow::onRestartSolverButtonClicked() {
    // Перезапуск солвера
    m_pChartWidget->OnSolveButtonClicked_SolverRestart();
}

void MainWindow::onZoomInButtonClicked() {
    if (!m_pChartWidget) {
        qDebug() << "Ошибка: m_pChartWidget не инициализирован";
        return;
    }
    m_pChartWidget->OnZoomInClicked();
}

void MainWindow::onZoomOutButtonClicked() {
    if (!m_pChartWidget) {
        qDebug() << "Ошибка: m_pChartWidget не инициализирован";
        return;
    }
    m_pChartWidget->OnZoomOutClicked();
}

void MainWindow::onOptimizationModeChanged() {
    if (m_pDurationRadioButton->isChecked()) {
        qDebug() << "Оптимизация длительности";
    } else if (m_pCostRadioButton->isChecked()) {
        qDebug() << "Оптимизация стоимости";
    } else if (m_pSetupsRadioButton->isChecked()) {
        qDebug() << "Оптимизация переналадок";
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    int toolbarHeight = m_pToolBar->height();
    m_pChartWidget->setToolbarHeight(toolbarHeight);
    QMainWindow::resizeEvent(event);
}
