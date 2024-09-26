#include "GanttChartWidget.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>

GanttChartWidget::GanttChartWidget(QWidget *pParent)
    : QWidget(pParent), m_iDraggedJob(-1), m_iDraggedMachine(-1), m_bJsMode(true) {
    m_pGanttChart = new GanttChart();
    Initialize();

    // Создаем тулбар и кнопки
    m_pToolBar = new QToolBar(this);
    //кнопка Запустить солвер из файл
    m_pSolveButton = new QPushButton("Запустить солвер из файла", this);
    // кнопка для отображения времени по рабочим
    m_pShowTimeButton = new QPushButton("Отобразить время по рабочим", this);

    // Добавляем кнопки на тулбар
    m_pToolBar->addWidget(m_pSolveButton);
    m_pToolBar->addWidget(m_pShowTimeButton);

    // Создаем вертикальный layout и добавляем тулбар и график в него
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);  // Убираем отступы
    layout->addWidget(m_pToolBar, 0, Qt::AlignLeft);  // Тулбар наверху, выравниваем влево
    layout->addStretch(1);  // Пространство для графика

    setLayout(layout);  // Устанавливаем layout для виджета

    // Подключаем сигнал нажатия кнопки к слоту
    connect(m_pSolveButton, &QPushButton::clicked, this, &GanttChartWidget::OnSolveButtonClicked_2);
    connect(m_pShowTimeButton, &QPushButton::clicked, this, &GanttChartWidget::onWorkersTimeButtonClicked);
    connect(m_pShowTimeButton, &QPushButton::clicked, this, &GanttChartWidget::DrawWorkersTimeChart); // Подключаем сигнал к слоту


}


GanttChartWidget::~GanttChartWidget() {
    delete m_pGanttChart;
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
        // Показываем новое название файла
        QMessageBox::information(this, "Файл выбран", "Вы выбрали файл: " + filename);
        LoadJsonData(filename);
    }
    else {
           // Показываем сообщение, что ничего не было выбрано
           QMessageBox::warning(this, "Нет файла", "Ничего не получено");
       }
}


void GanttChartWidget::OnSolveButtonClicked_2() {
    QMessageBox::information(this, "Файл не выбран", "перерисовывю на структурах: " );
    const QString filename;
    LoadJsonData_2(filename);
}


void GanttChartWidget::LoadJsonData(const QString &filename) {
    if (m_pGanttChart) {
        std::cout << "This file_name in LoadJsonData: " << filename.toStdString() << std::endl;
        m_pGanttChart->LoadJsonData(filename);
        DrawGanttChart(); // Перерисовываем диаграмму при загрузке данных
        update();
    }
}

void GanttChartWidget::LoadJsonData_2(const QString &filename) {
    if (m_pGanttChart) {
        std::cout << "This file_name in LoadJsonData: " << filename.toStdString() << std::endl;
        m_pGanttChart->LoadJsonData_2(filename);
        DrawGanttChart(); // Перерисовываем диаграмму при загрузке данных
        update();
    }
}

void GanttChartWidget::DrawGanttChart() {
    // Перед изменением изображения и вызовом QPainter убедимся, что изображение чистое
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);  // Обновляем размеры изображения
    m_oChartImage.fill(Qt::white);  // Очищаем изображение

    // Создаем QPainter для работы с m_oChartImage
    QPainter oPainter(&m_oChartImage);

    // Убедимся, что QPainter активен
    if (!oPainter.isActive()) {
        qWarning("QPainter on m_oChartImage is not active");
        return;
    }

    // Передаем размеры виджета в функцию DrawGanttChart
    if (m_pGanttChart) {
        m_pGanttChart->DrawGanttChart(&oPainter, width(), height());
    }

    // Обновляем виджет для отображения
    update();  // Обновляем экран
}

//void GanttChartWidget::DrawWorkersTimeChart() {
//    // Подготавливаем изображение
//    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
//    m_oChartImage.fill(Qt::white);
//    QPainter oPainter(&m_oChartImage);

//    if (!oPainter.isActive()) {
//        qWarning("QPainter on m_oChartImage is not active");
//        return;
//    }

//    if (m_pGanttChart) {
//        m_pGanttChart->DrawWorkersTimeChart(&oPainter, width(), height());
//    }

//    update(); // Обновляем виджет для отображения
//}

void GanttChartWidget::DrawWorkersTimeChart() {
    QPainter painter(&m_oWorkersImage);
    if (!painter.isActive()) {
        qWarning("QPainter on m_oWorkersImage is not active");
        return;
    }

    if (m_pGanttChart) {
        m_pGanttChart->DrawWorkersTimeChart(&painter, m_oWorkersImage.width(), m_oWorkersImage.height());
    }

    update();
}

//void GanttChartWidget::paintEvent(QPaintEvent *event) {
//    QPainter oPainter(this);

//    // Рисуем сохраненную диаграмму с учетом тулбара
//    int toolbarHeight = m_pToolBar->height();
//    oPainter.drawImage(0, toolbarHeight, m_oChartImage);
//}

void GanttChartWidget::paintEvent(QPaintEvent *event) {
    QPainter oPainter(this);

    // Рисуем сохраненную диаграмму с учетом тулбара
    int toolbarHeight = m_pToolBar->height();

    if (m_bDisplayingWorkersTimeChart) {
        oPainter.drawImage(0, toolbarHeight, m_oWorkersImage);  // Отрисовываем workers chart
    } else {
        oPainter.drawImage(0, toolbarHeight, m_oChartImage);  // Отрисовываем Gantt chart
    }
}



//void GanttChartWidget::resizeEvent(QResizeEvent *event) {
//    // Получаем высоту тулбара
//    int toolbarHeight = m_pToolBar->height();

//    // Изменяем размер m_oChartImage с учетом высоты тулбара
//    m_oChartImage = QImage(event->size().width(), event->size().height() - toolbarHeight, QImage::Format_ARGB32);
//    m_oChartImage.fill(Qt::white);

//    // Перерисовываем диаграмму
//    DrawGanttChart();
//}

//void GanttChartWidget::resizeEvent(QResizeEvent *event) {
//    int toolbarHeight = m_pToolBar->height();
//    QImage &targetImage = m_bDisplayingWorkersTimeChart ? m_oWorkersImage : m_oChartImage;
//    targetImage = QImage(event->size().width(), event->size().height() - toolbarHeight, QImage::Format_ARGB32);
//    targetImage.fill(Qt::white);

//    if (m_bDisplayingWorkersTimeChart) {
//        DrawWorkersTimeChart();
//    } else {
//        DrawGanttChart();
//    }

//    update();
//}

void GanttChartWidget::resizeEvent(QResizeEvent *event) {
    int toolbarHeight = m_pToolBar->height();

    // Обновляем размеры изображений для графиков
    m_oChartImage = QImage(event->size().width(), event->size().height() - toolbarHeight, QImage::Format_ARGB32);
    m_oWorkersImage = QImage(event->size().width(), event->size().height() - toolbarHeight, QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);
    m_oWorkersImage.fill(Qt::white);

    // Отрисовываем активный график
    if (m_bDisplayingWorkersTimeChart) {
        DrawWorkersTimeChart();
    } else {
        DrawGanttChart();
    }
}




void GanttChartWidget::onWorkersTimeButtonClicked() {
    m_bDisplayingWorkersTimeChart = !m_bDisplayingWorkersTimeChart;
    updateChart();
}

// Update the chart based on the current display flag
void GanttChartWidget::updateChart() {
    resizeEvent(new QResizeEvent(size(), size()));
}




void GanttChartWidget::mousePressEvent(QMouseEvent *event) {
    // Логика по кликам мыши и взаимодействию с диаграммой
}

void GanttChartWidget::mouseMoveEvent(QMouseEvent *event) {
    // Логика по перетаскиванию баров
}
