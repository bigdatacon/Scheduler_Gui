#include "GanttChartWidget.h"
#include <QVBoxLayout>
#include <QFileDialog>

GanttChartWidget::GanttChartWidget(QWidget *pParent)
    : QWidget(pParent), m_iDraggedJob(-1), m_iDraggedMachine(-1), m_bJsMode(true) {
    m_oLogic = new GanttChart();
    Initialize();

    // Создаем тулбар и кнопку
    m_pToolBar = new QToolBar(this);
    m_pSolveButton = new QPushButton("Запустить солвер из файла", this);

    // Добавляем кнопку на тулбар
    m_pToolBar->addWidget(m_pSolveButton);

    // Создаем вертикальный layout и добавляем тулбар и график в него
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);  // Убираем отступы
    layout->addWidget(m_pToolBar);  // Тулбар наверху
    layout->addStretch(1);  // Пространство для графика

    setLayout(layout);  // Устанавливаем layout для виджета

    // Подключаем сигнал нажатия кнопки к слоту
    connect(m_pSolveButton, &QPushButton::clicked, this, &GanttChartWidget::OnSolveButtonClicked);
}

GanttChartWidget::~GanttChartWidget() {
    delete m_oLogic;
}

void GanttChartWidget::Initialize() {
    resize(1424, 968);
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);
}

void GanttChartWidget::OnSolveButtonClicked() {
    // Здесь можно поместить логику для запуска солвера из файла
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл для солвера", "", "JSON Files (*.json);;All Files (*)");
    if (!filename.isEmpty()) {
        LoadJsonData(filename);
    }
}

void GanttChartWidget::LoadJsonData(const QString &filename) {
    if (m_oLogic) {
        m_oLogic->LoadJsonData(filename);
        DrawGanttChart(); // Перерисовываем диаграмму при загрузке данных
        update();
    }
}

void GanttChartWidget::DrawGanttChart() {
    // Создаем QPainter для работы с m_oChartImage
    QPainter oPainter(&m_oChartImage);

    // Убедимся, что QPainter активен
    if (!oPainter.isActive()) {
        qWarning("QPainter on m_oChartImage is not active");
        return;
    }

    // Передаем размеры виджета в функцию DrawGanttChart
    if (m_oLogic) {
        m_oLogic->DrawGanttChart(&oPainter, width(), height());
    }

    // Обновляем виджет
    update();  // Обновляем экран
}

void GanttChartWidget::paintEvent(QPaintEvent *event) {
    // Создаем QPainter для работы с виджетом
    QPainter oPainter(this);

    // Рисуем сохраненную диаграмму
    oPainter.drawImage(0, 0, m_oChartImage);
}

void GanttChartWidget::resizeEvent(QResizeEvent *event) {
    // Изменяем размер m_oChartImage при изменении размера виджета
    m_oChartImage = QImage(event->size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    // Перерисовываем диаграмму
    DrawGanttChart();
}

void GanttChartWidget::mousePressEvent(QMouseEvent *event) {
    // Логика по кликам мыши и взаимодействию с диаграммой
}

void GanttChartWidget::mouseMoveEvent(QMouseEvent *event) {
    // Логика по перетаскиванию баров
}
