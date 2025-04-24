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
#include <execinfo.h>  // Linux/macOS only

GanttChartWidget::GanttChartWidget(QWidget *pParent, GanttChart *pGanttChart,
                                     QPushButton *m_pZoomButton, int m_toolbarHeight,
                                     int statusBarHeight)
    : QWidget(pParent),
      m_pZoomButton(m_pZoomButton),
      m_toolbarHeight(m_toolbarHeight),
      m_pGanttChart(pGanttChart),

      m_istatusBarHeight(statusBarHeight),
      m_iDraggedJob(-1),
      m_iDraggedMachine(-1),
      m_bJsMode(true),
      m_offset(0, 0),
      m_lastMousePos(0, 0),
      m_bHandToolActive(false)
{

    if (!m_pGanttChart) {
        qDebug() << "Ошибка: передан nullptr в GanttChartWidget";
        return;
    }
    m_bendScroll = false;
    setMouseTracking(true); // Чтобы получать mouseMoveEvent без зажатых кнопок
    QSize viewportSize = parentWidget() ? parentWidget()->size() : QSize(800, 600);
    qDebug() << "Calculated viewport size from parent in cinstructor in GanttChartWidget :" << viewportSize;

    Initialize();
    QTimer::singleShot(900, this, SLOT(UpdateSize()));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    QSize viewportSize = parentWidget() ? parentWidget()->size() : QSize(800, 600);
    qDebug() << "Calculated viewport size from parent in cinstructor in Initialize :" << viewportSize;

    qDebug() << "width and height of picture in Initialize :" << width << height;
}


void GanttChartWidget::updateZoomedImages() {

    Initialize();  // Обновляем размеры изображений

    m_bManualResizeOnly = true;
    DrawGanttChart();
    DrawWorkersTimeChart();
    m_bManualResizeOnly = false;

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

}


void GanttChartWidget::resizeEvent(QResizeEvent *event) {
    update();
    QWidget::resizeEvent(event);
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

    m_pGanttChart->set_zoom(1.0);
//    m_fPreciseScrollOffset = QPointF(0, 0); // сбросить точный offset
    setPreciseOffset(QPointF(0, 0), __FUNCTION__);

    updateZoomedImages();
    UpdateSize();
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

    QPointF mousePosWidget = mapFromGlobal(QCursor::pos());


    // 👇 Считываем текущее точное смещение
    QPointF scrollOffset = m_fPreciseScrollOffset;

    // 👇 Вычисляем позицию точки под курсором на изображении ДО увеличения
    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / zoom;

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
    setPreciseOffset(newScrollOffset, __FUNCTION__);


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

    setPreciseOffset(QPointF(m_offset), __FUNCTION__);

    QPointF scrollOffset = m_fPreciseScrollOffset;

    // 👇 Получаем позицию курсора относительно самого виджета
     QPointF mousePosWidget = mapFromGlobal(QCursor::pos());

    // 👇 Позиция на изображении до зума
    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / zoom;


    // 👇 Применяем новый зум
    m_pGanttChart->set_zoom(newZoom);
    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));

    updateZoomedImages();
    UpdateSize();

    // 👇 Пересчитываем позицию после зума
    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;
    setPreciseOffset(newScrollOffset, __FUNCTION__);

    update();
}


void GanttChartWidget::OnZoomInClickedScroll(const QPointF &) {
        qDebug() << "in OnZoomInClickedScroll    imageSize :" << getCurrentImageSize() << "viewportSize:" << this->size();
    double currentZoom = m_pGanttChart->get_zoom();
    if (currentZoom >= 8.0) return;
    QPointF mousePosWidget = mapFromGlobal(QCursor::pos());

    QPointF scrollOffset = m_fPreciseScrollOffset;

    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / currentZoom;

    double newZoom = currentZoom + 0.05;
    m_pGanttChart->set_zoom(newZoom);
    updateZoomedImages();
    UpdateSize();

    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

//    m_fPreciseScrollOffset = newScrollOffset;
    setPreciseOffset(newScrollOffset, __FUNCTION__);
    qDebug() << "After setPreciseOffset: raw offset =" << m_offset;

    qDebug() << "After zoom-in precise offset =" << m_fPreciseScrollOffset << "raw offset =" << m_offset;



    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));
    update();
}

void GanttChartWidget::OnZoomOutClickedScroll(const QPointF &) {
    qDebug() << "in OnZoomOutClickedScroll    imageSize :" << getCurrentImageSize() << "viewportSize:" << this->size();

    double currentZoom = m_pGanttChart->get_zoom();
    if (currentZoom <= 1.0) {
        QMessageBox::warning(this, "Zoom Limit", "Zoom не может быть меньше 1.0");
        return;
    }

    QPointF mousePosWidget = mapFromGlobal(QCursor::pos());
    QPointF scrollOffset = m_fPreciseScrollOffset;
    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / currentZoom;

    double newZoom = std::max(currentZoom - 0.05, 1.0);
    m_pGanttChart->set_zoom(newZoom);
    updateZoomedImages();
    UpdateSize();

    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

    setPreciseOffset(newScrollOffset, __FUNCTION__);

    qDebug() << "After setPreciseOffset: raw offset =" << m_offset;
    qDebug() << "After zoom-out precise offset =" << m_fPreciseScrollOffset << "raw offset =" << m_offset;

    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));
    update();
}


//void GanttChartWidget::OnZoomOutClickedScroll(const QPointF &) {
//    double currentZoom = m_pGanttChart->get_zoom();
//    if (currentZoom <= 1.0) {
//        QMessageBox::warning(this, "Zoom Limit", "Zoom не может быть меньше 1.0");
//        return;
//    }

//    QPointF mousePosWidget = mapFromGlobal(QCursor::pos());

//    QPointF scrollOffset = m_fPreciseScrollOffset;

//    QPointF mouseImagePosBeforeZoom = (scrollOffset + mousePosWidget) / currentZoom;
//    double newZoom = std::max(currentZoom - 0.05, 1.0);

//    m_pGanttChart->set_zoom(newZoom);
//    updateZoomedImages();
//    UpdateSize();

//    QPointF mouseImagePosAfterZoom = mouseImagePosBeforeZoom * newZoom;
//    QPointF newScrollOffset = mouseImagePosAfterZoom - mousePosWidget;

//    m_fPreciseScrollOffset = newScrollOffset;

//    m_pZoomButton->setText("Zoom: " + QString::number(newZoom, 'f', 2));
//    update();
//}


double GanttChartWidget::scrollBarValueToDouble(QScrollBar *scrollBar, double zoom) const {
    return static_cast<double>(scrollBar->value()) / zoom;
}
int GanttChartWidget::doubleToScrollBarValue(double value, double zoom) const {
    return static_cast<int>(value * zoom);
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

    // Устанавливаем layout в диалоговом окне
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->addWidget(textEdit);
    dialog->setLayout(dialogLayout);

    // Показываем диалог
    dialog->exec();
}



void GanttChartWidget::UpdateSize() {
//    int width = VIRTUAL_SCREEN_WIDTH * m_pGanttChart->get_zoom();
//    int height = VIRTUAL_SCREEN_HEIGHT * m_pGanttChart->get_zoom()-m_istatusBarHeight*2 ;

//    setMinimumSize(width, height);
//    resize(width, height ); // Корректный расчет общей высотыв
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



QPoint GanttChartWidget::clampOffsetToValidRange(const QPoint& offset) const {
    QSize imageSize = getCurrentImageSize();
    QSize viewportSize = this->size();

    int minOffsetX = viewportSize.width() - imageSize.width();
    int minOffsetY = viewportSize.height() - imageSize.height();

    if (minOffsetX > 0) minOffsetX = 0;
    if (minOffsetY > 0) minOffsetY = 0;

    int clampedX = std::clamp(offset.x(), minOffsetX, 0);
    int clampedY = std::clamp(offset.y(), minOffsetY, 0);

    qDebug() << "[CLAMP] imageSize:" << imageSize
             << "viewportSize:" << viewportSize
             << "| before: offset =" << offset
             << "| minOffsetX =" << minOffsetX << "minOffsetY =" << minOffsetY
             << "| after: offset =" << QPoint(clampedX, clampedY);

    return QPoint(clampedX, clampedY);
}





void GanttChartWidget::mouseMoveEvent(QMouseEvent *event) {
    // Если активен режим перетаскивания (hand tool), выполняем существующую логику
//    qDebug() << "offset in mouseMoveEvent: " << m_offset;
    if (m_bHandToolActive) {
        QPoint currentMousePos = event->pos();
        QPoint offset = currentMousePos - m_lastMousePos;

        QPoint proposedOffset = m_offset + offset;
        m_offset = clampOffsetToValidRange(proposedOffset);
        setPreciseOffset(QPointF(m_offset), __FUNCTION__);


        m_lastMousePos = currentMousePos;
        update();
        event->accept();

    }

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


void GanttChartWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    QTimer::singleShot(300, this, SLOT(UpdateSize()));

    QSize viewportSize =parentWidget()->size();
    qDebug() << "💡 Реальный размер родителя после отображения in showEvent: " << viewportSize;
}


void GanttChartWidget::printStackTrace(const QString &message) {
    constexpr int MAX_FRAMES = 20;
    void* buffer[MAX_FRAMES];
    int size = backtrace(buffer, MAX_FRAMES);
    char** symbols = backtrace_symbols(buffer, size);

    qDebug() << "\n\n=== Stack trace for:" << message << "===";
    for (int i = 0; i < size; ++i) {
        qDebug() << symbols[i];
    }
    free(symbols);
    qDebug() << "====================";
}

#include <execinfo.h>

void GanttChartWidget::setPreciseOffset(QPointF offset, const QString &context) {
    qDebug() << "➡ setPreciseOffset called. context =" << context << ", offset =" << offset;

    m_fPreciseScrollOffset = offset;

    QSize imageSize = getCurrentImageSize();
    QSize viewportSize = parentWidget() ? parentWidget()->size() : size();

    qDebug() << "📐 imageSize:" << imageSize << "viewportSize:" << viewportSize;

    int proposedX = static_cast<int>(std::round(offset.x()));
    int proposedY = static_cast<int>(std::round(offset.y()));

    int maxOffsetX = std::max(0, viewportSize.width() - imageSize.width());
    int maxOffsetY = std::max(0, viewportSize.height() - imageSize.height());
    int minOffsetX = std::min(0, viewportSize.width() - imageSize.width());
    int minOffsetY = std::min(0, viewportSize.height() - imageSize.height());


//    int clampedX = std::clamp(proposedX, minOffsetX, maxOffsetX);
//    int clampedY = std::clamp(proposedY, minOffsetY, maxOffsetY);

    int clampedX;
    int clampedY;
    // clamp в обе стороны
    if (proposedX>0 && minOffsetX < 0){
        clampedX = std::clamp(-proposedX, minOffsetX, maxOffsetX);
    }
    else
    {
             clampedX = std::clamp(proposedX, minOffsetX, maxOffsetX);
        }

    if (proposedY>0 && minOffsetY < 0){
        clampedY = std::clamp(-proposedY, minOffsetY, maxOffsetY);
    }
    else
    {
            clampedY = std::clamp(proposedY, minOffsetY, maxOffsetY);
        }


    m_offset = QPoint(clampedX, clampedY);


    qDebug() << "🔢 Proposed offset: (" << proposedX << "," << proposedY << ")";
    qDebug() << "📏 MinOffsetX:" << minOffsetX << "MinOffsetY:" << minOffsetY;
    qDebug() << "✅ Clamped offset will be:" << m_offset;

    if (m_offset == QPoint(0, 0) && offset != QPointF(0, 0)) {
        qDebug() << "⚠️ m_offset обнулился несмотря на offset =" << offset;
        printStackTrace("Offset reset to (0, 0)");
    }

    qDebug() << "🎯 Установлен m_offset:" << m_offset;

    update();
}






QSize GanttChartWidget::getCurrentImageSize() const {
    int width = VIRTUAL_SCREEN_WIDTH * m_pGanttChart->get_zoom();
    int height = VIRTUAL_SCREEN_HEIGHT * m_pGanttChart->get_zoom();
    return QSize(width, height);
}





