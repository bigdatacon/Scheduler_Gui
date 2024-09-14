#include "GanttChartWidget.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>

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
    layout->addWidget(m_pToolBar, 0, Qt::AlignLeft);  // Тулбар наверху, выравниваем влево
    layout->addStretch(1);  // Пространство для графика

    setLayout(layout);  // Устанавливаем layout для виджета

    // Подключаем сигнал нажатия кнопки к слоту
//    connect(m_pSolveButton, &QPushButton::clicked, this, &GanttChartWidget::OnSolveButtonClicked);
    connect(m_pSolveButton, &QPushButton::clicked, this, &GanttChartWidget::OnSolveButtonClicked_2);
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
    if (m_oLogic) {
        std::cout << "This file_name in LoadJsonData: " << filename.toStdString() << std::endl;
        m_oLogic->LoadJsonData(filename);
        DrawGanttChart(); // Перерисовываем диаграмму при загрузке данных
        update();
    }
}

void GanttChartWidget::LoadJsonData_2(const QString &filename) {
    if (m_oLogic) {
        std::cout << "This file_name in LoadJsonData: " << filename.toStdString() << std::endl;
        m_oLogic->LoadJsonData_2(filename);
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
    if (m_oLogic) {
        m_oLogic->DrawGanttChart(&oPainter, width(), height());
    }

    // Обновляем виджет для отображения
    update();  // Обновляем экран
}


void GanttChartWidget::paintEvent(QPaintEvent *event) {
    QPainter oPainter(this);

    // Рисуем сохраненную диаграмму с учетом тулбара
    int toolbarHeight = m_pToolBar->height();
    oPainter.drawImage(0, toolbarHeight, m_oChartImage);
}


void GanttChartWidget::resizeEvent(QResizeEvent *event) {
    // Получаем высоту тулбара
    int toolbarHeight = m_pToolBar->height();

    // Изменяем размер m_oChartImage с учетом высоты тулбара
    m_oChartImage = QImage(event->size().width(), event->size().height() - toolbarHeight, QImage::Format_ARGB32);
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
