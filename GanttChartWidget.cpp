#include "GanttChartWidget.h"

GanttChartWidget::GanttChartWidget(QWidget *pParent)
    : QWidget(pParent), m_iDraggedJob(-1), m_iDraggedMachine(-1), m_bJsMode(true) {
    Initialize();
}

void GanttChartWidget::Initialize() {
    resize(1424, 968);
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);
}

void GanttChartWidget::LoadJsonData(const QString &filename) {
    m_oLogic.LoadJsonData(filename);
    DrawGanttChart();  // Перерисовываем диаграмму при загрузке данных
    update();
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
    m_oLogic.DrawGanttChart(&oPainter, width(), height());

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
