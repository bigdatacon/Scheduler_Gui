#include "GanttChartWidget.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>
#include <QDebug> // Подключите для использования qDebug
#include "GlobalState.h"
#include <QThread>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <algorithm> // Для std::clamp
#include <QCursor>
#include <cmath>



GanttChartWidget::GanttChartWidget(QWidget *pParent, GanttChart *pGanttChart,
                                     QPushButton *m_pZoomButton, int m_toolbarHeight, QScrollArea *pScrollArea,
                                     int statusBarHeight)
    : QWidget(pParent),
      m_pZoomButton(m_pZoomButton),
      m_toolbarHeight(m_toolbarHeight),
      m_pGanttChart(pGanttChart),
      m_pScrollArea(pScrollArea),
      m_istatusBarHeight(statusBarHeight),
      m_iDraggedJob(-1),
      m_iDraggedMachine(-1),
      m_bJsMode(true),
      m_offset(0, 0),
      m_lastMousePos(0, 0),
      m_bHandToolActive(false)
{
    qDebug() << "GanttChartWidget: m_pScrollArea =" << m_pScrollArea;
    if (!m_pGanttChart) {
        qDebug() << "Ошибка: передан nullptr в GanttChartWidget";
        return;
    }
    m_bendScroll = false;
    setMouseTracking(true); // Чтобы получать mouseMoveEvent без зажатых кнопок
    Initialize();
    QTimer::singleShot(900, this, SLOT(UpdateSize()));

    m_pScrollArea->setFrameShape(QFrame::NoFrame);
    m_pScrollArea->setContentsMargins(0, 0, 0, 0);
    m_pScrollArea->viewport()->setContentsMargins(0, 0, 0, 0);

//    connect(m_pScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
//        m_offset.setX(value);
//        m_offset = clampOffsetToValidRange(m_offset);
//        m_fPreciseScrollOffset.setX(m_offset.x());
//        update();
//    });

//    connect(m_pScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
//        m_offset.setY(value);
//        m_offset = clampOffsetToValidRange(m_offset);
//        m_fPreciseScrollOffset.setY(m_offset.y());
//        update();
//    });


    updateScrollBars();
}




GanttChartWidget::~GanttChartWidget() {
    delete m_pGanttChart;
}

void GanttChartWidget::Initialize() {

    int width = VIRTUAL_SCREEN_WIDTH * m_pGanttChart->get_zoom();
    int height = VIRTUAL_SCREEN_HEIGHT* m_pGanttChart->get_zoom()-m_istatusBarHeight*2;

    m_oChartImage = QImage(width, height, QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    m_oWorkersImage = QImage(width, height, QImage::Format_ARGB32);
    m_oWorkersImage.fill(Qt::white);
}


void GanttChartWidget::updateZoomedImages() {
    m_bSkipOffsetClampInUpdateSize = true;
    Initialize();  // Обновляем размеры изображений
    DrawGanttChart();  // Перерисовываем диаграмму Гантта
    DrawWorkersTimeChart();  // Перерисовываем диаграмму ресурсов
    update();  // Обновляем отображение
}


void GanttChartWidget::OnSolveButtonClicked() {
    m_filename = QFileDialog::getOpenFileName(this, "Выберите файл для солвера", "", "TXT Files (*.txt);;All Files (*)");

    if (!m_filename.isEmpty()) {
        // Создаем немодальное окно с информацией о выбранном файле
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setText("Файл выбран");
        msgBox->setInformativeText("Вы выбрали файл: " + m_filename);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setModal(false); // Делаем окно немодальным
        msgBox->show();

        if ( g_pSolver != NULL )
        {
            delete g_pSolver;
            g_pSolver = NULL;
        }

        // Загружаем данные из файла
        LoadData(m_filename);
    } else {
        // Создаем немодальное предупреждение о том, что файл не выбран
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setText("Нет файла");
        msgBox->setInformativeText("Ничего не получено");
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setModal(false); // Делаем окно немодальным
        msgBox->show();
    }
}


void GanttChartWidget::OnSolveButtonClicked_SolverRestart() {
    LoadData(m_filename);
}


void GanttChartWidget::LoadData(const QString &filename) {
    m_filename = filename;

    if (m_pGanttChart) {
//        std::cout << "This file_name in LoadJsonData: " << filename.toStdString() << std::endl;
        m_pGanttChart->LoadData(filename);
        DrawGanttChart(); // Перерисовываем диаграмму при загрузке данных
        DrawWorkersTimeChart(); // сразу рисую диаграмму по ресурсам
        update();
    }
}

void GanttChartWidget::DrawGanttChart() {
    int width = VIRTUAL_SCREEN_WIDTH * m_pGanttChart->get_zoom();
    int height = VIRTUAL_SCREEN_HEIGHT* m_pGanttChart->get_zoom()-m_istatusBarHeight*2;

    m_oChartImage = QImage(width, height, QImage::Format_ARGB32); // Обновляем размеры изображения
    m_oChartImage.fill(Qt::white);

    QPainter oPainter(&m_oChartImage);

    if (!oPainter.isActive()) {
        qWarning("QPainter on m_oChartImage is not active");
        return;
    }

    // Передаем размеры виджета в функцию DrawGanttChart
    if (m_pGanttChart) {
        m_pGanttChart->DrawGanttChart(&oPainter, width, height);
    }

    b_yet_draw_ghant_chart = true;
    UpdateSize(); // Обновляем размеры виджета
    update();  // Обновляем экран
}


void GanttChartWidget::DrawWorkersTimeChart() {
//    qDebug() << "DrawWorkersTimeChart Обновленная высота тулбара: " << m_toolbarHeight;
    int width = VIRTUAL_SCREEN_WIDTH * m_pGanttChart->get_zoom();
//    int height = (VIRTUAL_SCREEN_HEIGHT-m_toolbarHeight ) * m_pGanttChart->get_zoom();
    int height = VIRTUAL_SCREEN_HEIGHT * m_pGanttChart->get_zoom()-m_istatusBarHeight*2;

    m_oWorkersImage = QImage(width, height, QImage::Format_ARGB32); // Обновляем размеры изображения
    m_oWorkersImage.fill(Qt::white);

    QPainter painter(&m_oWorkersImage);

    if (!painter.isActive()) {
        qWarning("QPainter on m_oWorkersImage is not active");
        return;
    }

    if (m_pGanttChart) {
        m_pGanttChart->DrawWorkersTimeChart(&painter, width, height);
    }

    b_yet_draw_workers_chart = true;
    UpdateSize(); // Обновляем размеры виджета
    update();
}


void GanttChartWidget::paintEvent(QPaintEvent *event) {
    QPainter oPainter(this);
    oPainter.setRenderHint(QPainter::Antialiasing);

    // Полная область виджета (включая тулбар)
    QRect visibleRect(0, 0, width(), height());
    // Заполняем фон белым цветом
    oPainter.fillRect(visibleRect, Qt::white);

    // Определяем, что отображать: график ресурсов или диаграмму Гантта
    if (m_bDisplayingWorkersTimeChart) {
        // Рисуем диаграмму утилизации ресурсов
        drawImageWithOffset(oPainter, m_oWorkersImage, visibleRect);

    } else {
        // Рисуем диаграмму Гантта
        drawImageWithOffset(oPainter, m_oChartImage, visibleRect);

    }
}

void GanttChartWidget::drawImageWithOffset(QPainter &oPainter, const QImage &sourceImage, const QRect &visibleRect) {
    // Размер изображения
    QRect imageRect = sourceImage.rect();

    // Рассчитываем sourceRect — какую часть изображения надо показать
    QRect sourceRect = visibleRect.translated(-m_offset).intersected(imageRect);

    // Если нечего рисовать — выходим
    if (sourceRect.isEmpty()) return;

    // Рассчитываем, куда рисовать — targetRect на экране
    QRect targetRect = sourceRect.translated(m_offset);

    // Рисуем только видимую часть изображения
    oPainter.drawImage(targetRect, sourceImage, sourceRect);

    // Отладка
//    qDebug() << "[DRAW OFFSET] m_offset:" << m_offset;
//    qDebug() << "[DRAW OFFSET] sourceRect:" << sourceRect;
//    qDebug() << "[DRAW OFFSET] targetRect:" << targetRect;
}


//void GanttChartWidget::drawImageWithOffset(QPainter &oPainter, const QImage &sourceImage, const QRect &visibleRect) {
//    // Размер изображения (оригинала)
//    QRect sourceImageRect = sourceImage.rect();

//    // Ограничиваем смещение таким образом, чтобы оно корректно учитывало выход за пределы
//    QRect adjustedSourceRect = sourceImageRect.translated(-m_offset);

//    // Проверяем размеры adjustedSourceRect (можно позволить отрицательные значения)
//    if (visibleRect.isEmpty()) {
//        qWarning() << "Видимый прямоугольник пуст. Рисование пропущено.";
//        return;
//    }

//    // Рассчитываем целевой прямоугольник на экране (видимая область с учетом смещения)
//    QRect targetRect = QRect(visibleRect.topLeft() + m_offset, sourceImageRect.size());

//    // Рисуем изображение. Здесь мы используем drawImage напрямую без обрезания
//    oPainter.drawImage(targetRect, sourceImage);
//}


//void GanttChartWidget::resizeEvent(QResizeEvent *event) {
//    int availableWidth = event->size().width();
//    int availableHeight = event->size().height();
//    int adjustedHeight = availableHeight; // если статусная строка уже вне centralWidget, то она не входит
//    update();
//    QWidget::resizeEvent(event);
//}

void GanttChartWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    UpdateSize();
    update();
//    qDebug() << "[DRAW] m_fPreciseScrollOffset:" << m_fPreciseScrollOffset;
//    qDebug() << "[DRAW] Scrollbar value (h/v):"
//             << findScrollArea()->horizontalScrollBar()->value()
//             << findScrollArea()->verticalScrollBar()->value();
}



void GanttChartWidget::onWorkersTimeButtonClicked() {
    m_bDisplayingWorkersTimeChart = !m_bDisplayingWorkersTimeChart;
    updateChart();
}

// Update the chart based on the current display flag
void GanttChartWidget::updateChart() {
    resizeEvent(new QResizeEvent(size(), size()));
}


void GanttChartWidget::setZoom(double zoomLevel) {
    if (!m_pGanttChart) {
        qDebug() << "Ошибка: m_pGanttChart не инициализирован.";
        return;
    }

//    m_pGanttChart->set_zoom(zoomLevel);
//    m_pZoomButton->setText("Zoom: " + QString::number(zoomLevel, 'f', 1));
//    updateZoomedImages();
//    UpdateSize(); // Обновляем размеры виджета
//    updateScrollBars(); // Обновляем состояние полос прокрутки

    m_pGanttChart->set_zoom(1.0);
    m_fPreciseScrollOffset = QPointF(0, 0); // сбросить точный offset
    updateZoomedImages();
    UpdateSize();
    findScrollArea()->horizontalScrollBar()->setValue(0);
    findScrollArea()->verticalScrollBar()->setValue(0);

    updateScrollBars();
    m_pZoomButton->setText("Zoom: 1.0");
    update();

}



void GanttChartWidget::OnZoomInClicked() {
    if (!m_pGanttChart) {
        qDebug() << "Ошибка: m_pGanttChart не инициализирован";
        return;
    }

    double zoom = m_pGanttChart->get_zoom();
    if (zoom >= 8.0) return;

    QScrollArea* scrollArea = findScrollArea();
    if (!scrollArea) return;

    // 👇 Получаем позицию курсора относительно viewport
    QPointF mousePosWidget = scrollArea->viewport()->mapFromGlobal(QCursor::pos());

    // 👇 Считываем текущее точное смещение
    QPointF scrollOffset = m_fPreciseScrollOffset;

    // 👇 Вычисляем позицию точки под курсором на изображении ДО увеличения
    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / zoom;

//    qDebug() << "[Viewport->mapFromGlobal(QCursor::pos())]:" << mousePosWidget;
//    qDebug() << "Текущий зум: " << zoom;
//    qDebug() << "scrollOffset (scrollbars):" << scrollOffset;
//    qDebug() << "mouseImagePosBeforeZoom (точка на изображении):" << mouseImagePosBeforeZoom;

    // 👇 Увеличиваем зум
    double newZoom = zoom + 0.5;
    m_pGanttChart->set_zoom(newZoom);
    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));

    updateZoomedImages();
    UpdateSize();

    // 👇 Вычисляем позицию этой же точки после зума
    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;

    // 👇 Считаем, насколько нужно прокрутить, чтобы курсор остался над этой точкой
    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

    // 👇 Сохраняем точное смещение
    m_fPreciseScrollOffset = newScrollOffset;

    QScrollBar* hScroll = scrollArea->horizontalScrollBar();
    QScrollBar* vScroll = scrollArea->verticalScrollBar();
    hScroll->setValue(static_cast<int>(std::round(newScrollOffset.x())));
    vScroll->setValue(static_cast<int>(std::round(newScrollOffset.y())));

//    qDebug() << "mouseImagePosAfterZoom (после зума):" << mouseImagePosAfterZoom;
//    qDebug() << "newScrollOffset (чтобы оставить курсор над той же точкой):" << newScrollOffset;
//    qDebug() << "Зум после увеличения: " << newZoom;

    update();
}

void GanttChartWidget::OnZoomOutClicked() {
    if (!m_pGanttChart) {
        qDebug() << "Ошибка: m_pGanttChart не инициализирован";
        return;
    }

    double zoom = m_pGanttChart->get_zoom();
    if (zoom <= 1.0) return;

    double newZoom = zoom - 0.5;
    if (newZoom < 1.0) newZoom = 1.0;

    QScrollArea* scrollArea = findScrollArea();
    if (!scrollArea) return;

    QScrollBar* hScroll = scrollArea->horizontalScrollBar();
    QScrollBar* vScroll = scrollArea->verticalScrollBar();

    // 👇 Обновляем текущее точное смещение перед зумом
    m_fPreciseScrollOffset = QPointF(hScroll->value(), vScroll->value());
    QPointF scrollOffset = m_fPreciseScrollOffset;

    // 👇 Позиция курсора на viewport
    QPointF mousePosWidget = scrollArea->viewport()->mapFromGlobal(QCursor::pos());

    // 👇 Позиция на изображении до зума
    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / zoom;

//    qDebug() << "[Viewport->mapFromGlobal(QCursor::pos())]:" << mousePosWidget;
//    qDebug() << "Текущий зум: " << zoom;
//    qDebug() << "scrollOffset (scrollbars):" << scrollOffset;
//    qDebug() << "mouseImagePosBeforeZoom (точка на изображении):" << mouseImagePosBeforeZoom;

    // 👇 Применяем новый зум
    m_pGanttChart->set_zoom(newZoom);
    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));

    updateZoomedImages();
    UpdateSize();

    // 👇 Пересчитываем позицию после зума
    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

    // 👇 Сохраняем и применяем scroll offset
    m_fPreciseScrollOffset = newScrollOffset;
    hScroll->setValue(static_cast<int>(std::round(newScrollOffset.x())));
    vScroll->setValue(static_cast<int>(std::round(newScrollOffset.y())));

//    qDebug() << "mouseImagePosAfterZoom (после зума):" << mouseImagePosAfterZoom;
//    qDebug() << "newScrollOffset (чтобы оставить курсор над той же точкой):" << newScrollOffset;
//    qDebug() << "Зум после уменьшения: " << newZoom;

    update();
}


//void GanttChartWidget::OnZoomInClickedScroll(const QPointF &) {
//    QScrollArea *scrollArea = findScrollArea();
//    if (!scrollArea) return;

//    QScrollBar *hScroll = scrollArea->horizontalScrollBar();
//    QScrollBar *vScroll = scrollArea->verticalScrollBar();

//    //  offset
//    m_fPreciseScrollOffset = QPointF(hScroll->value(), vScroll->value());

//    double currentZoom = m_pGanttChart->get_zoom();
//    if (currentZoom >= 8.0) return;

//    if (currentZoom == 1.0) {
//        m_fPreciseScrollOffset = QPointF(hScroll->value(), vScroll->value());
//    }
//    // 1. Позиция мыши
//    QPointF mousePosWidget = scrollArea->viewport()->mapFromGlobal(QCursor::pos());
//    QPointF scrollOffset = m_fPreciseScrollOffset;
//    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / currentZoom;

//    // 2. Зум
//    double newZoom = currentZoom + 0.05;
//    m_pGanttChart->set_zoom(newZoom);
//    m_bSkipOffsetClampInUpdateSize = true;

//    updateZoomedImages();

//    m_bSkipOffsetClampInUpdateSize = true;
//    UpdateSize();
//    m_bSkipOffsetClampInUpdateSize = false;


//    // 3. Новый scrollOffset
//    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
//    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

//    // 🧩 4. Ограничение вручную
//    QSize imageSize = m_bDisplayingWorkersTimeChart ? m_oWorkersImage.size() : m_oChartImage.size();
//    QSize viewportSize = scrollArea->viewport()->size();

//    double maxOffsetX = 0;
//    double minOffsetX = viewportSize.width() - imageSize.width();
//    double maxOffsetY = 0;
//    double minOffsetY = viewportSize.height() - imageSize.height();

//    newScrollOffset.setX(std::clamp(newScrollOffset.x(), minOffsetX, maxOffsetX));
//    newScrollOffset.setY(std::clamp(newScrollOffset.y(), minOffsetY, maxOffsetY));

//    // 5. Сохраняем
//    m_fPreciseScrollOffset = newScrollOffset;

//    hScroll->setValue(static_cast<int>(std::round(newScrollOffset.x())));
//    vScroll->setValue(static_cast<int>(std::round(newScrollOffset.y())));

//    m_fPreciseScrollOffset = QPointF(
//        hScroll->value(),
//        vScroll->value()
//    );

//    qDebug() << "[ZoomOut] Принудительно обновили m_fPreciseScrollOffset после scrollBar.setValue:" << m_fPreciseScrollOffset;


//    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));
//    updateScrollBars();
//    update();
//}


//void GanttChartWidget::OnZoomOutClickedScroll(const QPointF &) {
//    QScrollArea *scrollArea = findScrollArea();
//    if (!scrollArea) return;

//    QScrollBar *hScroll = scrollArea->horizontalScrollBar();
//    QScrollBar *vScroll = scrollArea->verticalScrollBar();
//    //  offset
//    m_fPreciseScrollOffset = QPointF(hScroll->value(), vScroll->value());

//    double currentZoom = m_pGanttChart->get_zoom();
//    if (currentZoom <= 1.0) {
//        QMessageBox::warning(this, "Zoom Limit", "Zoom не может быть меньше 1.0");
//        return;
//    }

//    if (currentZoom == 1.0) {
//        m_fPreciseScrollOffset = QPointF(hScroll->value(), vScroll->value());
//    }

//    // 1. Получаем актуальную позицию мыши
//    QPointF mousePosWidget = scrollArea->viewport()->mapFromGlobal(QCursor::pos());
//    QPointF scrollOffset = m_fPreciseScrollOffset;

//    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / currentZoom;
////    QPointF mouseImagePosBeforeZoom = m_fMouseImagePosBeforeZoom;

////    qDebug() << "[Viewport->mapFromGlobal(QCursor::pos())]:" << mousePosWidget;
////    qDebug() << "Текущий зум: " << currentZoom;
////    qDebug() << "scrollOffset (scrollbars):" << scrollOffset;
////    qDebug() << "mouseImagePosBeforeZoom (точка на изображении):" << mouseImagePosBeforeZoom;

//    // 2. Уменьшаем зум
//    double newZoom = std::max(currentZoom - 0.05, 1.0);  // Защита от выхода ниже 1.0
//    m_pGanttChart->set_zoom(newZoom);
//    m_bSkipOffsetClampInUpdateSize = true;
//    updateZoomedImages();

//    m_bSkipOffsetClampInUpdateSize = true;
//    UpdateSize();
//    m_bSkipOffsetClampInUpdateSize = false;


//    // 3. Пересчитываем новые координаты и точный scroll
//    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
//    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

////    qDebug() << "[Уменьшил зум и всё обновил]";
////    qDebug() << "mouseImagePosAfterZoom (после зума):" << mouseImagePosAfterZoom;
////    qDebug() << "newScrollOffset (чтобы оставить курсор над той же точкой):" << newScrollOffset;
////    qDebug() << "Зум после уменьшения: " << newZoom;

//    // 4. Устанавливаем точное значение
//    m_fPreciseScrollOffset = newScrollOffset;

//    // 5. Применяем округление при установке в скроллбары
//    hScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.x())));
//    vScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.y())));

//    m_fPreciseScrollOffset = QPointF(
//        hScroll->value(),
//        vScroll->value()
//    );

//    qDebug() << "[ZoomOut] Принудительно обновили m_fPreciseScrollOffset после scrollBar.setValue:" << m_fPreciseScrollOffset;


//    // 6. Обновляем интерфейс
//    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));
//    updateScrollBars();
//    update();
//}

void GanttChartWidget::OnZoomInClickedScroll(const QPointF &) {
    QScrollArea *scrollArea = findScrollArea();
    if (!scrollArea) return;

    QScrollBar *hScroll = scrollArea->horizontalScrollBar();
    QScrollBar *vScroll = scrollArea->verticalScrollBar();

    double currentZoom = m_pGanttChart->get_zoom();
    if (currentZoom >= 8.0) return;

    // 1. Получаем актуальную позицию мыши
    QPointF mousePosWidget = scrollArea->viewport()->mapFromGlobal(QCursor::pos());
    QPointF scrollOffset = m_fPreciseScrollOffset;

    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / currentZoom;
//    QPointF mouseImagePosBeforeZoom = m_fMouseImagePosBeforeZoom;


//    qDebug() << "[Viewport->mapFromGlobal(QCursor::pos())]:" << mousePosWidget;
//    qDebug() << "Текущий зум: " << currentZoom;
//    qDebug() << "scrollOffset (scrollbars):" << scrollOffset;
//    qDebug() << "mouseImagePosBeforeZoom (точка на изображении):" << mouseImagePosBeforeZoom;

    // 2. Увеличиваем зум
    double newZoom = currentZoom + 0.05;
    m_pGanttChart->set_zoom(newZoom);
    updateZoomedImages();
    m_bManualResizeOnly = true;
    UpdateSize();
    m_bManualResizeOnly = false;


    // 3. Перевычисляем новые координаты и точный scroll
    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

//    qDebug() << "[Увеличил зум и всё обновил]";
//    qDebug() << "mouseImagePosAfterZoom (после зума):" << mouseImagePosAfterZoom;
//    qDebug() << "newScrollOffset (чтобы оставить курсор над той же точкой):" << newScrollOffset;
//    qDebug() << "Зум после увеличения: " << newZoom;

    // 4. Устанавливаем точное значение (оставляем в памяти — не трогаем потом!)
    m_fPreciseScrollOffset = newScrollOffset;

    // 5. Применяем округление ТОЛЬКО при установке значения в скроллбары
    hScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.x())));
    vScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.y())));

    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));
    updateScrollBars();
    update();
}



void GanttChartWidget::OnZoomOutClickedScroll(const QPointF &) {
    QScrollArea *scrollArea = findScrollArea();
    if (!scrollArea) return;

    QScrollBar *hScroll = scrollArea->horizontalScrollBar();
    QScrollBar *vScroll = scrollArea->verticalScrollBar();

    double currentZoom = m_pGanttChart->get_zoom();
    if (currentZoom <= 1.0) {
        QMessageBox::warning(this, "Zoom Limit", "Zoom не может быть меньше 1.0");
        return;
    }

    // 1. Получаем актуальную позицию мыши
    QPointF mousePosWidget = scrollArea->viewport()->mapFromGlobal(QCursor::pos());
    QPointF scrollOffset = m_fPreciseScrollOffset;

    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / currentZoom;
//    QPointF mouseImagePosBeforeZoom = m_fMouseImagePosBeforeZoom;

//    qDebug() << "[Viewport->mapFromGlobal(QCursor::pos())]:" << mousePosWidget;
//    qDebug() << "Текущий зум: " << currentZoom;
//    qDebug() << "scrollOffset (scrollbars):" << scrollOffset;
//    qDebug() << "mouseImagePosBeforeZoom (точка на изображении):" << mouseImagePosBeforeZoom;

    // 2. Уменьшаем зум
    double newZoom = std::max(currentZoom - 0.05, 1.0);  // Защита от выхода ниже 1.0
    m_pGanttChart->set_zoom(newZoom);
    updateZoomedImages();
    m_bManualResizeOnly = true;
    UpdateSize();
    m_bManualResizeOnly = false;


    // 3. Пересчитываем новые координаты и точный scroll
    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

//    qDebug() << "[Уменьшил зум и всё обновил]";
//    qDebug() << "mouseImagePosAfterZoom (после зума):" << mouseImagePosAfterZoom;
//    qDebug() << "newScrollOffset (чтобы оставить курсор над той же точкой):" << newScrollOffset;
//    qDebug() << "Зум после уменьшения: " << newZoom;

    // 4. Устанавливаем точное значение
    m_fPreciseScrollOffset = newScrollOffset;

    // 5. Применяем округление при установке в скроллбары
    hScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.x())));
    vScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.y())));

    // 6. Обновляем интерфейс
    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));
    updateScrollBars();
    update();
}



double GanttChartWidget::scrollBarValueToDouble(QScrollBar *scrollBar, double zoom) const {
    return static_cast<double>(scrollBar->value()) / zoom;
}
int GanttChartWidget::doubleToScrollBarValue(double value, double zoom) const {
    return static_cast<int>(value * zoom);
}



QScrollArea* GanttChartWidget::findScrollArea() const {
    QWidget *currentParent = parentWidget();  // Начинаем с текущего родителя
    while (currentParent) {  // Проходим по всем уровням родителей
        QScrollArea *scrollArea = qobject_cast<QScrollArea *>(currentParent);
        if (scrollArea) {
            return scrollArea;  // Если нашли QScrollArea, возвращаем его
        }
        currentParent = currentParent->parentWidget();  // Переходим к следующему родителю
    }
    return nullptr;  // Если QScrollArea не найден, возвращаем nullptr
}


void GanttChartWidget::mousePressEvent(QMouseEvent *event) {
    // Обработчик "ладони" (перетаскивание)
    if (event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton) {
        if (m_pGanttChart->get_zoom() != 1.0) {
            m_bHandToolActive = true;
            m_lastMousePos = event->pos();
            setCursor(Qt::ClosedHandCursor);  // Изменяем курсор на "ладонь"
            event->accept();
            return;  // Прерываем дальнейшую обработку событий
        }
    }

    QPoint clickPos = event->pos() - m_offset;
    bool barClicked = false;
    bool clickedLower = false;
    bool clickedUpper = false;

    if (!m_pGanttChart) {
        qDebug() << "m_pGanttChart не инициализирован";
        return;
    }

    // Получаем операции
    auto &msOperations = m_pGanttChart->getMsOperations();
    auto &jsOperations = m_pGanttChart->getJsOperations();
    auto &vUnavData = m_pGanttChart->getUnavData();


    // --- Логика для кликов по нижнему графику (jsOperations) ---
    if (event->button() == Qt::LeftButton) {
        for (auto &sOp : jsOperations) {
            if (sOp.rect.contains(clickPos)) {
                // Сброс выделения
                for (auto &jobOp : jsOperations) jobOp.bHighlighted = false;
                for (auto &machineOp : msOperations) machineOp.bHighlighted = false;
                sOp.bHighlighted = true;

                // Выделяем связанные бары на верхнем графике
                for (auto &machineIndex : sOp.vMachinesIndexes) {
                    for (auto &mOp : msOperations) {
                        if (mOp.iMachine == machineIndex &&
                            mOp.iJob == sOp.iJob &&
                            mOp.iStart == sOp.iStart &&
                            mOp.iFinish == sOp.iFinish) {
                            mOp.bHighlighted = true;
                        }
                    }
                }
                // Выделяем предшественников только на нижнем графике
                {
                    const auto &preds = g_pSolver->GetOperations()[sOp.iOperation]->GetPredecessors();
                    int countPredLower = 0;
                    for (auto &lowerOp : jsOperations) {
                        if (lowerOp.iJob == sOp.iJob) {
                            string opID = g_pSolver->GetOperations()[lowerOp.iOperation]->GetID();
                            for (const auto &pred : preds) {
                                if (get<0>(pred) == opID) {
                                    lowerOp.bHighlighted = true;
                                    countPredLower++;
                                    break;
                                }
                            }
                        }
                    }
                }
                clickedLower = true;
                barClicked = true;
                break;
            }
        }
    }

    // --- Логика для кликов по верхнему графику ---
    if (!barClicked && event->button() == Qt::LeftButton) {
        for (auto &mOp : msOperations) {
            if (mOp.rect.contains(clickPos)) {
                // Сохраняем параметры нажатого элемента
                int clickedJob = mOp.iJob;
                int clickedOperation = mOp.iOperation;
                int clickedMachine = mOp.iMachine;
                // Сброс выделения
                for (auto &jobOp : jsOperations) jobOp.bHighlighted = false;
                for (auto &machineOp : msOperations) machineOp.bHighlighted = false;

                mOp.bHighlighted = true;

                // Первый проход: выделяем связанные нижние бары для этого элемента
                int countLinked = 0;
                for (auto &sOp : jsOperations) {
                    if (sOp.iJob == clickedJob &&
                        std::find(sOp.vMachinesIndexes.begin(), sOp.vMachinesIndexes.end(), clickedMachine) != sOp.vMachinesIndexes.end()) {
                        sOp.bHighlighted = true;
                        countLinked++;

                    }
                }

                // Второй проход: выделяем дополнительные верхние бары с тем же job и операцией,
                // Очистим векторы перед новым кликом
                m_pGanttChart->m_vMainOperations.clear();
                m_pGanttChart->m_vGroupedPredecessors.clear();

                // Выделяем дополнительные верхние бары с тем же job и операцией, но с другим номером машины и совпадающими временами.
                int countAdditional = 0;
                for (auto &upperOp : msOperations) {
                    if (upperOp.iJob == clickedJob &&
                        upperOp.iOperation == clickedOperation &&
                        upperOp.iStart == mOp.iStart &&
                        upperOp.iFinish == mOp.iFinish)
                    {
                        upperOp.bHighlighted = true;
                        m_pGanttChart->m_vMainOperations.push_back(&upperOp);  // добавляем в основной вектор
                        countAdditional++;

                    }
                }
                qDebug() << "Всего основных/дополнительных верхних баров:" << countAdditional;

                // Группируем предшественников по времени начала
                const auto &preds = g_pSolver->GetOperations()[clickedOperation]->GetPredecessors();
                int countPredUpper = 0;
                std::map<int, std::vector<SResourceOperation*>> timeToGroup;

                for (auto &upperOp : msOperations) {
                    std::string opID = g_pSolver->GetOperations()[upperOp.iOperation]->GetID();
                    for (const auto &pred : preds) {
                        if (get<0>(pred) == opID) {
                            upperOp.bHighlighted = true;
                            timeToGroup[upperOp.iStart].push_back(&upperOp);
                            countPredUpper++;

                            break;
                        }
                    }
                }
                for (auto &[startTime, group] : timeToGroup) {
                    m_pGanttChart->m_vGroupedPredecessors.push_back(group);
                }

                clickedUpper = true;
                barClicked = true;
                break;
            }
        }
    }

    if (!barClicked) {
        qDebug() << "Клик по пустой области";
        for (auto &jobOp : jsOperations) jobOp.bHighlighted = false;
        for (auto &machineOp : msOperations) machineOp.bHighlighted = false;
    }

    DrawGanttChart();
    update();

    if (event->button() == Qt::RightButton) {
        handleRightClick(clickPos, jsOperations, msOperations, vUnavData);
    }
}

void GanttChartWidget::handleRightClick(QPoint clickPos, std::vector<SJobOperation> &jsOperations, std::vector<SResourceOperation> &msOperations, std::vector<sUnavData> &vUnavData) {
    int resourceCount = g_pSolver->GetResources().size();

    // Проверка кликов по прямоугольникам недоступности
    for (const auto& unav : vUnavData) {
        if (unav.rect.contains(clickPos)) {
            QTime baseTime(8, 0);
            QString start = baseTime.addSecs(unav.time_start * 60).toString("HH:mm");
            QString finish = baseTime.addSecs(unav.time_end * 60).toString("HH:mm");

            QString info;
            if (unav.resource_num >= 0 && unav.resource_num < resourceCount) {
                info = QString("Ресурс: %1 (%2)\nНачало: %3\nКонец: %4")
                        .arg(QString::fromStdString(g_pSolver->GetResources()[unav.resource_num]->GetName()))
                        .arg(QString::fromStdString(g_pSolver->GetResources()[unav.resource_num]->GetType()))
                        .arg(start)
                        .arg(finish);
            } else {
                info = QString("Недопустимый индекс ресурса: %1").arg(unav.resource_num);
                qWarning() << "Недопустимый unav.resource_num:" << unav.resource_num;
            }

            QMessageBox* msgBox = new QMessageBox(this);
            msgBox->setText("Информация о недоступности ресурса");
            msgBox->setInformativeText(info);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setModal(false);
            msgBox->show();
            return;
        }
    }

    // Клик по нижнему графику (операции по работам)
    for (auto &sOp : jsOperations) {
        if (sOp.rect.contains(clickPos)) {
            qDebug() << "Правый клик по бару: Д" << sOp.iJob;

            QTime baseTime(8, 0);
            QString start = baseTime.addSecs(sOp.iStart * 60).toString("HH:mm");
            QString finish = baseTime.addSecs(sOp.iFinish * 60).toString("HH:mm");

            QString info = QString("%1\nID: %2")
                .arg(QString::fromStdString(g_pSolver->GetJobs()[sOp.iJob]->GetID()))
                .arg(QString::fromStdString(g_pSolver->GetOperations()[sOp.iOperation]->GetID()));

            auto predecs = g_pSolver->GetOperations()[sOp.iOperation]->GetPredecessors();
            if (!predecs.empty()) {
                info.append(" (");
                for (size_t i = 0; i < predecs.size(); ++i) {
                    const auto& [id, type, val] = predecs[i];
                    info.append(QString::fromStdString(id)).append(" ")
                        .append(QChar(type)).append(" ")
                        .append(QString::number(val));
                    if (i < predecs.size() - 1)
                        info.append(", ");
                }
                info.append(")");
            }

            info.append(QString("\nТип: %1\nРазряд: %2\nНачало: %3\nКонец: %4\nДлительность: %5 мин\nРесурсы: ")
                        .arg(QString::fromStdString(g_pSolver->GetOperations()[sOp.iOperation]->GetType()))
                        .arg(g_pSolver->GetOperations()[sOp.iOperation]->GetDischarge())
                        .arg(start)
                        .arg(finish)
                        .arg(sOp.iFinish - sOp.iStart));

            for (int machineIndex : sOp.vMachinesIndexes) {
                if (machineIndex >= 0 && machineIndex < resourceCount) {
                    info.append(QString("%1, ").arg(QString::fromStdString(g_pSolver->GetResources()[machineIndex]->GetName())));
                } else {
                    info.append(QString("[недопустимый %1], ").arg(machineIndex));
                    qWarning() << "Недопустимый machineIndex в sOp:" << machineIndex;
                }
            }

            if (!sOp.vMachinesIndexes.empty()) {
                info.chop(2);
            }

            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setText("Информация об операции");
            msgBox->setInformativeText(info);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setModal(false);
            msgBox->show();
            return;
        }
    }

    // Клик по верхнему графику (операции по ресурсам)
    for (auto &mOp : msOperations) {
        if (mOp.rect.contains(clickPos)) {
            qDebug() << "Правый клик по бару на верхнем графике: Рабочий" << mOp.iMachine;

            QTime baseTime(8, 0);
            QString start = baseTime.addSecs(mOp.iStart * 60).toString("HH:mm");
            QString finish = baseTime.addSecs(mOp.iFinish * 60).toString("HH:mm");

            QString info = QString("%1\nID: %2")
                .arg(QString::fromStdString(g_pSolver->GetJobs()[mOp.iJob]->GetID()))
                .arg(QString::fromStdString(g_pSolver->GetOperations()[mOp.iOperation]->GetID()));

            auto predecs = g_pSolver->GetOperations()[mOp.iOperation]->GetPredecessors();
            if (!predecs.empty()) {
                info.append(" (");
                for (size_t i = 0; i < predecs.size(); ++i) {
                    const auto& [id, type, val] = predecs[i];
                    info.append(QString::fromStdString(id)).append(" ")
                        .append(QChar(type)).append(" ")
                        .append(QString::number(val));
                    if (i < predecs.size() - 1)
                        info.append(", ");
                }
                info.append(")");
            }

            if (mOp.iMachine >= 0 && mOp.iMachine < resourceCount) {
                auto resource = g_pSolver->GetResources()[mOp.iMachine];

                info.append(QString("\nТип: %1\nРазряд: %2\nНачало: %3\nКонец: %4\nДлительность: %5 мин\nПереналадка: %6 мин\nРесурс: %7 (%8)")
                        .arg(QString::fromStdString(g_pSolver->GetOperations()[mOp.iOperation]->GetType()))
                        .arg(g_pSolver->GetOperations()[mOp.iOperation]->GetDischarge())
                        .arg(start)
                        .arg(finish)
                        .arg(mOp.iFinish - mOp.iStart)
                        .arg(mOp.iSetupTime)
                        .arg(QString::fromStdString(resource->GetName()))
                        .arg(QString::fromStdString(resource->GetType())));

                auto useCases = resource->GetUseCases();
                auto reqResources = g_pSolver->GetOperations()[mOp.iOperation]->GetRequiredResources();

                info.append(" (");
                bool flag = false;
                for (auto& uc : useCases) {
                    if (flag) info.append(", ");
                    flag = true;

                    bool highlighted = false;
                    for (const auto& req : reqResources) {
                        if (uc.first == req.first && uc.second >= g_pSolver->GetOperations()[mOp.iOperation]->GetDischarge()) {
                            info.append(QString("*%1 %2*").arg(QString::fromStdString(uc.first)).arg(uc.second));
                            highlighted = true;
                            break;
                        }
                    }
                    if (!highlighted)
                        info.append(QString("%1 %2").arg(QString::fromStdString(uc.first)).arg(uc.second));
                }
                info.append(")");

            } else {
                info.append(QString("\n❗ Недопустимый индекс ресурса: %1").arg(mOp.iMachine));
                qWarning() << "Недопустимый mOp.iMachine:" << mOp.iMachine;
            }

            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setText("Информация об операции");
            msgBox->setInformativeText(info);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setModal(false);
            msgBox->show();
            return;
        }
    }
}

void GanttChartWidget::OnShowScheduleMetricsClicked() {
    // Инициализация данных структуры ScheduleMetrics
    // Создаем новое диалоговое окно для отображения данных
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Метрики расписания");

    // Создаем текстовое поле для отображения данных
    QTextEdit* textEdit = new QTextEdit(dialog);
    textEdit->setReadOnly(true);  // Делаем поле только для чтения

    // Отключаем перенос строк
    textEdit->setWordWrapMode(QTextOption::NoWrap);

    // Добавляем текст
    QString dataText;

    // Имя файла
    QString fileName = "Schedule.txt";

    // Создание объекта QFile для работы с файлом
    QFile file(fileName);

    // Проверка, что файл открылся
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Не удалось открыть файл!" << std::endl;
        return;
    }

    // Чтение данных из файла и сохранение в переменную dataText
    QTextStream in(&file);
    dataText = in.readAll();

    // Закрываем файл
    file.close();
    textEdit->setText(dataText);

    // Добавляем вертикальную и горизонтальную полосу прокрутки
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Устанавливаем layout в диалоговом окне
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->addWidget(textEdit);
    dialog->setLayout(dialogLayout);

    // Показываем диалог
    dialog->exec();
}



//void GanttChartWidget::UpdateSize() {
//    int width = VIRTUAL_SCREEN_WIDTH * m_pGanttChart->get_zoom();
//    int height = VIRTUAL_SCREEN_HEIGHT * m_pGanttChart->get_zoom()-m_istatusBarHeight*2 ;

//    setMinimumSize(width, height);
//    resize(width, height ); // Корректный расчет общей высоты
//}


void GanttChartWidget::UpdateSize() {
    int width = VIRTUAL_SCREEN_WIDTH * m_pGanttChart->get_zoom();
    int height = VIRTUAL_SCREEN_HEIGHT * m_pGanttChart->get_zoom() - m_istatusBarHeight * 2;

    setMinimumSize(width, height);
    resize(width, height);

    qDebug() << "[UpdateSize] called";
    qDebug() << "Zoom =" << m_pGanttChart->get_zoom();
    qDebug() << "Widget size will be:" << width << "x" << height;
    qDebug() << "m_bSkipOffsetClampInUpdateSize =" << m_bSkipOffsetClampInUpdateSize;
    qDebug() << "m_offset before clamp:" << m_offset;
    qDebug() << "m_fPreciseScrollOffset before clamp:" << m_fPreciseScrollOffset;

    if (m_bManualResizeOnly) {
        qDebug() << "[UpdateSize] Skipping all offset modifications. Preserving scroll offset exactly.";
        return;
    }

    QSize imageSize(width, height);
    QSize viewportSize = m_pScrollArea->viewport()->size();

    qDebug() << "viewportSize =" << viewportSize;
    qDebug() << "imageSize =" << imageSize;

    int minOffsetX = (imageSize.width() > viewportSize.width()) ? viewportSize.width() - imageSize.width() : 0;
    int minOffsetY = (imageSize.height() > viewportSize.height()) ? viewportSize.height() - imageSize.height() : 0;
    int maxOffsetX = 0;
    int maxOffsetY = 0;

    m_offset.setX(std::clamp(static_cast<int>(std::round(m_fPreciseScrollOffset.x())), minOffsetX, maxOffsetX));
    m_offset.setY(std::clamp(static_cast<int>(std::round(m_fPreciseScrollOffset.y())), minOffsetY, maxOffsetY));
    m_fPreciseScrollOffset = QPointF(m_offset);

    qDebug() << "m_offset after clamp:" << m_offset;
    qDebug() << "m_fPreciseScrollOffset after clamp:" << m_fPreciseScrollOffset;
}









void GanttChartWidget::setToolbarHeight(int height) {
    m_toolbarHeight = height;
    update(); // Перерисовываем виджет при изменении высоты тулбара
};



void GanttChartWidget::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() == Qt::ControlModifier) {
        // Координаты указателя мыши относительно виджета
        QPointF mousePosWidget = event->position();

        // Определяем направление прокрутки
        int delta = event->angleDelta().y();
        if (delta > 0) {
            OnZoomInClickedScroll(mousePosWidget);  // Передаем координаты мыши
        } else if (delta < 0) {
            OnZoomOutClickedScroll(mousePosWidget);  // Передаем координаты мыши
        }

        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}



void GanttChartWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (m_bHandToolActive && event->button() == Qt::LeftButton) {
        m_bHandToolActive = false;
        setCursor(Qt::ArrowCursor);  // Возвращаем стандартный курсор
        event->accept();
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}


QPoint GanttChartWidget::clampOffsetToValidRange(QPoint offset) const {
    QSize imageSize = m_bDisplayingWorkersTimeChart ? m_oWorkersImage.size() : m_oChartImage.size();
    QSize viewportSize = m_pScrollArea->viewport()->size();

    int minOffsetX = (imageSize.width() > viewportSize.width()) ? viewportSize.width() - imageSize.width() : 0;
    int minOffsetY = (imageSize.height() > viewportSize.height()) ? viewportSize.height() - imageSize.height() : 0;

    int maxOffsetX = 0;
    int maxOffsetY = 0;

    return QPoint(
        std::clamp(offset.x(), minOffsetX, maxOffsetX),
        std::clamp(offset.y(), minOffsetY, maxOffsetY)
    );
}

void GanttChartWidget::mouseMoveEvent(QMouseEvent *event) {
    // Если активен режим перетаскивания (hand tool), выполняем существующую логикуd
//    if (m_bHandToolActive) {
//        QPoint currentMousePos = event->pos();
//        QPoint offset = currentMousePos - m_lastMousePos;
//        m_offset += offset;
//        // (Ваш существующий код для ограничения смещения и обновления полос прокрутки)
//        m_lastMousePos = currentMousePos;
//        update();
//        event->accept();
//    }

    if (m_bHandToolActive) {
        if (m_bSkipOffsetClampInUpdateSize){
            qDebug() << "m_bSkipOffsetClampInUpdateSize = true:" << m_bSkipOffsetClampInUpdateSize;
        }

        QPoint currentMousePos = event->pos();
        QPoint offset = currentMousePos - m_lastMousePos;

        QPoint proposedOffset = m_offset + offset;
        m_offset = clampOffsetToValidRange(proposedOffset);
        m_fPreciseScrollOffset = QPointF(m_offset);

        m_lastMousePos = currentMousePos;
        update();

        m_pScrollArea->horizontalScrollBar()->setValue(m_offset.x());
        m_pScrollArea->verticalScrollBar()->setValue(m_offset.y());

        event->accept();
    }


//    if (m_bHandToolActive) {
//        QPoint currentMousePos = event->pos();
//        QPoint offset = currentMousePos - m_lastMousePos;

//        qDebug() << "[HAND] m_offset before += offset" << m_offset;
//        m_offset += offset;
//        qDebug() << "[HAND] m_offset after += offset" << m_offset;

//        QSize imageSize = m_bDisplayingWorkersTimeChart ? m_oWorkersImage.size() : m_oChartImage.size();
//        QSize viewportSize = m_pScrollArea->viewport()->size();

//        int imageLeft = m_offset.x();
//        int imageRight = imageLeft + imageSize.width();
//        int imageBottom = m_offset.y() + imageSize.height();

//        int viewportRight = viewportSize.width();
//        int viewportBottom = viewportSize.height();

////        int minOffsetX = viewportRight - imageSize.width()-imageLeft; // Правый край изображения >= правого края viewport
//        int minOffsetX = viewportRight - imageSize.width(); // Правый край изображения >= правого края viewport
//        int maxOffsetX = 0;                                 // Левая граница изображения не вылезает за левый край

//        int minOffsetY = viewportBottom - imageSize.height();
////        int minOffsetY = viewportBottom - imageSize.height()- m_offset.y();
//        int maxOffsetY = 0;



//        qDebug() << "[CHECK] imageLeft =" << imageLeft << " imageRight =" << imageRight;
//        qDebug() << "[CHECK] viewportRight =" << viewportRight;
//        qDebug() << "[CHECK] imageBottom =" << imageBottom << " viewportBottom =" << viewportBottom;

//        qDebug() << "[CLAMP RANGE] X: from" << minOffsetX << "to" << maxOffsetX;
//        qDebug() << "[CLAMP RANGE] Y: from" << minOffsetY << "to" << maxOffsetY;

//        m_offset.setX(std::clamp(m_offset.x(), minOffsetX, maxOffsetX));
//        m_offset.setY(std::clamp(m_offset.y(), minOffsetY, maxOffsetY));

//        qDebug() << "[HAND] m_offset after clamp:" << m_offset;

//        m_lastMousePos = currentMousePos;
//        update();
//        // Синхронизируем точный offset и scrollBar'ы после ручного перетаскивания
//        m_fPreciseScrollOffset = QPointF(m_offset);  // теперь scroll и offset синхронизированы

//        QScrollBar* hScroll = m_pScrollArea->horizontalScrollBar();
//        QScrollBar* vScroll = m_pScrollArea->verticalScrollBar();

//        hScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.x())));
//        vScroll->setValue(static_cast<int>(std::round(m_fPreciseScrollOffset.y())));


//        event->accept();
//    }









    else {
        // Если не перетаскиваем, проверяем позицию курсора для вывода информации о баре.
        QPoint pos = event->pos() - m_offset; // компенсируем смещение

        QString statusText;
        bool found = false;

        // Сначала проверяем периоды недоступности (vUnavData)
        for (const auto &unav : m_pGanttChart->getUnavData()) {
            if (unav.rect.contains(pos)) {
                QTime baseTime(8, 0);
                QString start = (baseTime.addSecs(unav.time_start * 60)).toString("HH:mm");
                QString finish = (baseTime.addSecs(unav.time_end * 60)).toString("HH:mm");
                statusText = QString("Период недоступности Ресурса: %1 (%2)\nНачало: %3, Конец: %4")
                                .arg(QString::fromStdString(g_pSolver->GetResources()[unav.resource_num - 1]->GetName()))
                                .arg(QString::fromStdString(g_pSolver->GetResources()[unav.resource_num - 1]->GetType()))
                                .arg(start).arg(finish);
                found = true;
                break;
            }
        }

        // Если не найдено в недоступных периодах, проверяем обычные бары (jsOperations)
        if (!found) {
            for (const auto &sOp : m_pGanttChart->getJsOperations()) {
                if (sOp.rect.contains(pos)) {
                    QTime baseTime(8, 0);
                    QString start = (baseTime.addSecs(sOp.iStart * 60)).toString("HH:mm");
                    QString finish = (baseTime.addSecs(sOp.iFinish * 60)).toString("HH:mm");
                    statusText = QString("Работа: %1\nОперация: %2\nНачало: %3, Конец: %4")
//                                    .arg(QString::fromStdString(g_pSolver->GetJobs()[sOp.iJob - 1]->GetID()))
                                    .arg(QString::fromStdString(g_pSolver->GetJobs()[sOp.iJob ]->GetID()))
                                    .arg(QString::fromStdString(g_pSolver->GetOperations()[sOp.iOperation]->GetID()))
                                    .arg(start).arg(finish);
                    found = true;
                    break;
                }
            }
        }

        // Если все еще не найдено – проверяем бары из msOperations (например, для верхнего графика)
        if (!found) {
            for (const auto &mOp : m_pGanttChart->getMsOperations()) {
                if (mOp.rect.contains(pos)) {
                    QTime baseTime(8, 0);
                    QString start = (baseTime.addSecs(mOp.iStart * 60)).toString("HH:mm");
                    QString finish = (baseTime.addSecs(mOp.iFinish * 60)).toString("HH:mm");
                    statusText = QString("Рабочий: %1\nОперация: %2\nНачало: %3, Конец: %4")
                                    .arg(mOp.iMachine)
                                    .arg(QString::fromStdString(g_pSolver->GetOperations()[mOp.iOperation]->GetID()))
                                    .arg(start).arg(finish);
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            statusText = "Пустая область";
        }

        // Отправляем сформированное сообщение в MainWindow
        emit statusTextChanged(statusText);

        // Передаем событие базовому обработчику
        QWidget::mouseMoveEvent(event);
    }
}


void GanttChartWidget::updateScrollBars() {
    if (!m_pScrollArea || !m_pGanttChart) {
        qDebug() << "Ошибка: m_pScrollArea или m_pGanttChart не инициализированы.";
        return;
    }

    double zoom = m_pGanttChart->get_zoom();

    if (zoom == 1.0) {
        // Скрываем полосы прокрутки полностью
        m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // Сбрасываем значения и смещения
        m_pScrollArea->horizontalScrollBar()->setValue(0);
        m_pScrollArea->verticalScrollBar()->setValue(0);

        m_offset = QPoint(0, 0);
        m_fPreciseScrollOffset = QPointF(0.0, 0.0);

    } else {
        // Включаем отображение и активацию скроллбаров
        m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        m_pScrollArea->horizontalScrollBar()->setEnabled(true);
        m_pScrollArea->verticalScrollBar()->setEnabled(true);
    }

    update();  // Перерисовать, если нужно
}


//void GanttChartWidget::updateScrollBars() {
//    if (!m_pScrollArea) {
//        qDebug() << "Ошибка: m_pScrollArea == nullptr. Область прокрутки не задана.";
//        return; // Проверяем, что область прокрутки существует
//    }

//    if (!m_pGanttChart) {
//        qDebug() << "Ошибка: m_pGanttChart == nullptr. Объект диаграммы Ганта не задан.";
//        return; // Проверяем, что объект диаграммы Ганта существует
//    }

//    double zoom = m_pGanttChart->get_zoom();

//    if (zoom == 1.0) {

//        // Отключаем возможность двигать полосы прокрутки
//        m_pScrollArea->horizontalScrollBar()->setEnabled(false);
//        m_pScrollArea->horizontalScrollBar()->setValue(m_pScrollArea->horizontalScrollBar()->minimum());


//        m_pScrollArea->verticalScrollBar()->setEnabled(false);
//        m_pScrollArea->verticalScrollBar()->setValue(m_pScrollArea->verticalScrollBar()->minimum());

//        // Сбрасываем смещение к начальному значению
//        m_offset = QPoint(0, 0);
//        m_fPreciseScrollOffset = QPointF(0.0, 0.0);

//        // Перерисовываем виджет, чтобы обновить его состояние
//        update();
//    } else {
////        qDebug() << "Зум не равен 1. Включаем полосы прокрутки.";

//        // Включаем полосы прокрутки
//        m_pScrollArea->horizontalScrollBar()->setEnabled(true);
//        m_pScrollArea->verticalScrollBar()->setEnabled(true);

//    }
//}



void GanttChartWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    QTimer::singleShot(300, this, SLOT(UpdateSize()));
}



