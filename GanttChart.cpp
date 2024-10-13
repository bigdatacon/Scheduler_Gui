#include "GanttChart.h"
#include "JsonReader.h"
#include <random>
#include <QDebug>
#include <iostream>
#include <QMap>
#include <tuple>
#include <set>

void GanttChart::LoadJsonData(const QString &sFilename) {
    JsonReader oReader;
    oReader.ReadOperationsFromFile(sFilename,  m_vJsOperations_cont, m_vMsOperations_cont, m_ScheduleMetrics);
    InitializeColors();
}


void GanttChart::LoadJsonData_2(const QString &sFilename) {
    JsonReader oReader;
    oReader.ReadOperationsFromFile_2(sFilename,  m_vJsOperations_cont, m_vMsOperations_cont, m_ScheduleMetrics);
    InitializeColors();
}

void GanttChart::InitializeColors() {
    // Очистка предыдущих цветов
    m_umapJobColors.clear();

    // Набор основных цветов
    std::vector<QColor> primaryColors = {
        QColor(255, 0, 0),   // Красный
        QColor(0, 255, 0),   // Зеленый
        QColor(0, 0, 255),   // Синий
        QColor(255, 255, 0), // Желтый
        QColor(255, 0, 255), // Пурпурный
        QColor(0, 255, 255), // Голубой
        QColor(128, 0, 128), // Фиолетовый
        QColor(255, 165, 0), // Оранжевый
        QColor(0, 128, 0),   // Темно-зеленый
        QColor(128, 128, 128)// Серый
    };

    // Применение к каждому цвету уменьшения насыщенности
    for (auto &color : primaryColors) {
        int h, s, v;
        color.getHsv(&h, &s, &v);  // Получаем текущие значения Hue, Saturation, Value

        // Уменьшаем насыщенность до 70% от исходной
        s = std::min(255, (int)(s * 0.7));  // 70% от исходной насыщенности

        // Уменьшаем яркость до 90% от исходной
        v = std::min(255, (int)(v * 0.9));  // 90% от исходной яркости

        // Применяем изменения обратно в цвет
        color.setHsv(h, s, v);

        // Устанавливаем непрозрачность на уровне 90%
        color.setAlpha(230);  // Прозрачность 90% (230 из 255)
    }

    // Счетчик для отслеживания, когда закончатся основные цвета
    int primaryColorIndex = 0;

    // Генератор для случайных цветов
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // Создание уникальных цветов для каждого job
    for (const auto &sOp : m_vJsOperations_cont) {
        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
            if (primaryColorIndex < primaryColors.size()) {
                // Используем основные цвета
                m_umapJobColors[sOp.iJob] = primaryColors[primaryColorIndex];
                primaryColorIndex++;
            } else {
                // Если основные цвета закончились, генерируем случайные полутона
                QColor randomColor(dis(gen), dis(gen), dis(gen));

                // Уменьшаем насыщенность случайных цветов
                int h, s, v;
                randomColor.getHsv(&h, &s, &v);
                s = std::min(255, (int)(s * 0.7));  // Уменьшаем насыщенность
                v = std::min(255, (int)(v * 0.9));  // Уменьшаем яркость
                randomColor.setHsv(h, s, v);
                randomColor.setAlpha(230);  // Устанавливаем непрозрачность

                m_umapJobColors[sOp.iJob] = randomColor;
            }
        }
    }
}


std::tuple<int, int, int> GanttChart::calculateMaxValues() {
    // Рассчитываем максимальное значение Finish для ограничения оси X
    int iMaxFinish = 0;
    for (const auto &op : m_vJsOperations_cont) {
        if (op.iFinish > iMaxFinish) {
            iMaxFinish = op.iFinish;
        }
    }
    for (const auto &op : m_vMsOperations_cont) {
        if (op.iFinish > iMaxFinish) {
            iMaxFinish = op.iFinish;
        }
    }
    if (iMaxFinish == 0) {
        iMaxFinish = 1;
    }

    // Определение максимального значения machine для верхнего графика и job для нижнего графика
    int iMaxMachine = 0;
    int iMaxJob = 0;
    for (const auto &op : m_vJsOperations_cont) {
        for (const auto &machineIndex : op.vMachinesIndexes) {
            if (machineIndex > iMaxMachine) {
                iMaxMachine = machineIndex;
            }
        }
    }

    for (const auto &op : m_vMsOperations_cont) {
        if (op.iJob > iMaxJob) {
            iMaxJob = op.iJob;
        }
    }

    // Возвращаем результат в виде кортежа (tuple)
    return std::make_tuple(iMaxFinish, iMaxMachine, iMaxJob);
}


void GanttChart::DrawGanttChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight) {
    // -- данные по максмальному числу машин и прочего - ///
    auto [iMaxFinish_f, iMaxMachine, iMaxJob] = calculateMaxValues();
    int iMaxFinish = iMaxFinish_f;
    int fMachineRowCount = iMaxMachine ;  // Количество строк для машин
    int fJobRowCount = iMaxJob ;          // Количество строк для задач
     // ------------------------------------------------ ///


    // -------------------------- ОПРЕДЕЛЕНИЕ ШРИФТОВ И ПЁР ----------------------------
    QFont titleFont;
    QFont axisLabelFont;
    QFont dynamicFont;
    QFont verticalFont;
    QFont verticalFontRotate;


    // Вычисляем средний размер экрана для более сбалансированного расчета шрифта
    int iAverageScreenSize = static_cast<int>((iScreenWidth + iScreenHeight) / 2.5);

    // Определение минимального размера шрифта
    int iFontSize = std::max(8, static_cast<int>(iAverageScreenSize * 0.01));

    // Шрифт для заголовков
    titleFont.setPointSize(std::max(9, static_cast<int>(iAverageScreenSize * 0.013)));
    titleFont.setBold(true);

    // Шрифт для подписей на осях (рабочие/детали)
    verticalFont.setPointSize(std::max(8, static_cast<int>(iAverageScreenSize * 0.01)));

    // Шрифт для динамических подписей (на барах)
    dynamicFont.setPointSize(std::max(7, static_cast<int>(iAverageScreenSize * 0.007)));
    dynamicFont.setBold(true);

    // Шрифт для подписей осей (цифры)
    axisLabelFont.setPointSize(iFontSize);

     // Шрифт повернутых подписей графиков
    verticalFontRotate.setPointSize(iFontSize);
    verticalFontRotate.setBold(true);

    // Определение перьев для сетки и осей
    QPen axisPen(Qt::black, 2);    // Для осей и заголовков
    QPen gridPen(QColor(0, 0, 0, 50));  // Для сетки
    QPen textPen(Qt::black, 1);    // Для текста

    // Перо для "Время (мин)" (чуть прозрачное и жирное)
    QPen timeLabelPen(QColor(0, 0, 0, 150));  // Черный с прозрачностью 150
    timeLabelPen.setWidth(1);                 // Толщина линии
    timeLabelPen.setStyle(Qt::SolidLine);      // Сплошная линия
    timeLabelPen.setCapStyle(Qt::SquareCap);   // Стиль линий

    //--------------------------------------------------------------------------//

    //-------------------------отступы-----------------------------------------//

    int ioffsetFromVertical =  std::max(static_cast<int>(iScreenHeight * 0.05), 50);
    int ioffsetFromSide =  std::max(static_cast<int>(iScreenWidth * 0.05), 50);

    int iScaleFactorX = (iScreenWidth - ioffsetFromSide*2) / iMaxFinish;
    //---------------------------------------------------------------------------//

    // Перерисовываем изображение с новыми размерами окна
    QImage m_oChartImage = QImage(iScreenWidth, iScreenHeight, QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);


    // Расчет доступной высоты для графиков, учитывая новый отступ снизу
//    int availableHeight = iScreenHeight - ioffsetFromVertical*2 - ioffsetFromVertical*0.5*6;
    int availableHeight = iScreenHeight - ioffsetFromVertical*2 - (ioffsetFromVertical*0.5 * 6 + pPainter->fontMetrics().height());


    float totalRowCount = fMachineRowCount + fJobRowCount;
    float spacingFactor = 1.1;  // С учетом 10% отступа между барами

    // Расчет процентной доли от availableHeight для минимальной и максимальной высоты бара
    float minBarHeightPercentage = 0.01; // 1% от availableHeight
    float maxBarHeightPercentage = 0.10; // 10% от availableHeight

    int minBarHeight = static_cast<int>(availableHeight * minBarHeightPercentage);
    int maxBarHeight = static_cast<int>(availableHeight * maxBarHeightPercentage);

    // Вычисляем начальное значение высоты бара, основываясь на общем количестве строк и доступной высоте
    int iMachineHeight = static_cast<int>(availableHeight / (totalRowCount * spacingFactor));
    // Корректируем высоту баров, чтобы она не выходила за установленные процентные пределы
    iMachineHeight = std::max(minBarHeight, std::min(maxBarHeight, iMachineHeight));

    int iOffsetYJs =  ioffsetFromVertical*2  ;
    int iOffsetYMs = iMachineHeight * 2 + fMachineRowCount * iMachineHeight + ioffsetFromVertical*2 + ioffsetFromVertical*0.7;

    // Подписи графиков
    QFontMetrics oMetrics(pPainter->font());
    int midPointMachine = ioffsetFromSide + (iScreenWidth - ioffsetFromSide) / 2 - oMetrics.horizontalAdvance("Диаграмма по рабочим") / 2;

    int midPointJob = ioffsetFromSide + (iScreenWidth - ioffsetFromSide) / 2 -  oMetrics.horizontalAdvance("Диаграмма по деталям") / 2;

    // Устанавливаем шрифт и перо для заголовков
    pPainter->setFont(titleFont);
    pPainter->setPen(axisPen);

    // Отрисовка заголовков графиков
    pPainter->drawText(midPointMachine, iOffsetYJs- ioffsetFromVertical, "Диаграмма по рабочим");
    pPainter->drawText(midPointJob, iOffsetYMs - iScreenHeight * 0.01, "Диаграмма по деталям");

    // Устанавливаю шрифт  для время мин
    pPainter->setFont(axisLabelFont);
    pPainter->setPen(timeLabelPen);

//    pPainter->drawText(midPointMachineDuration, iOffsetYJs - ioffsetFromVertical*0.5, "Длительность:");
            // Создаём строку с данными из ScheduleMetrics
    QString scheduleMetricsText = QString("Длительность: %1, стоимость: %2")
        .arg(m_ScheduleMetrics.iScheduleTime)
        .arg(m_ScheduleMetrics.iScheduleCost);

    // Используем эту строку для отрисовки текста
    int midPointMachineDuration = ioffsetFromSide + (iScreenWidth - ioffsetFromSide) / 2 - oMetrics.horizontalAdvance(scheduleMetricsText) / 2;
    pPainter->drawText(midPointMachineDuration, iOffsetYJs - ioffsetFromVertical * 0.5, scheduleMetricsText);

    qDebug() << "iOffsetYJs:" << iOffsetYJs << "iOffsetYMs:" << iOffsetYMs;
    qDebug() << "fMachineRowCount:" << fMachineRowCount << "fJobRowCount:" << fJobRowCount;

//    // Центрирование подписи "Время (мин)" между графиками
    int timeLabelX = ioffsetFromSide + (iScreenWidth - ioffsetFromSide) / 2 - ioffsetFromSide / 2;
    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
    pPainter->drawText(timeLabelX, iOffsetYJs + fMachineRowCount * iMachineHeight + ioffsetFromVertical*0.5, "Время (мин)");


    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
    int labelPositionY = iOffsetYMs + fJobRowCount * iMachineHeight + ioffsetFromVertical*0.5;
    int availableSpaceForLabel = iScreenHeight - labelPositionY;

    // Проверяем, достаточно ли места для отступа снизу
    if (availableSpaceForLabel <   std::max(ioffsetFromVertical, 20)) {
        labelPositionY = iScreenHeight - std::max(ioffsetFromVertical, 20);  // Поднимаем подпись вверх, чтобы обеспечить отступ снизу
    }

    pPainter->drawText(timeLabelX, labelPositionY, "Время (мин)");

    int timeLabelYBottom =iOffsetYMs + fJobRowCount * iMachineHeight + ioffsetFromVertical;
    // Убедимся, что подпись не заезжает на график
    if (timeLabelYBottom > iScreenHeight - 10) { // Если подпись слишком низко, поднимаем выше
        timeLabelYBottom = iScreenHeight - 10;
        pPainter->drawText(timeLabelX, timeLabelYBottom, "Время (мин)");
    }

     // рассчитываю где ось x для верхнего графика
    int iYPos_workers = iOffsetYJs + (fMachineRowCount-1 + 0.5) * iMachineHeight; // позиция для оси х по рабочим
    int iYPos_details = iOffsetYMs + (fJobRowCount-1 + 0.5) * iMachineHeight; // позиция для оси х по деталям

    pPainter->setFont(axisLabelFont);


    int lastDrawnPosition = -1;  // Переменная для хранения последней позиции, на которой был нарисован текст
    int step = 10;
    if (iMaxFinish > 100 && iMaxFinish <= 300) {
        step = 20;
    } else if (iMaxFinish > 300 && iMaxFinish <= 1000) {
        step = 50;
    } else if (iMaxFinish > 1000) {
        step = 100;
    }
    for (int i = 0; i <= iMaxFinish; i += step) {
        int currentPosition = ioffsetFromSide + i * iScaleFactorX;

        // Рассчитываем ширину текста для текущего значения
        int textWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(i));

        // Если разница между текущей позицией и последней позицией меньше ширины текста, пропускаем отрисовку
        if (lastDrawnPosition != -1 && (currentPosition - lastDrawnPosition) < textWidth) {
            i += step;  // Пропускаем одну метку
            continue;
        }

        // Рисуем линии и текст
        pPainter->setPen(gridPen);
        pPainter->drawLine(currentPosition, iOffsetYJs, currentPosition, iYPos_workers);
        pPainter->drawLine(currentPosition, iOffsetYMs, currentPosition, iYPos_details);

        pPainter->setPen(textPen);
        pPainter->drawText(currentPosition - textWidth / 2, iOffsetYJs + fMachineRowCount * iMachineHeight + ioffsetFromVertical * 0.15, QString::number(i));
        pPainter->drawText(currentPosition - textWidth / 2, iOffsetYMs + fJobRowCount * iMachineHeight + ioffsetFromVertical * 0.15, QString::number(i));

        // Обновляем последнюю отрисованную позицию
        lastDrawnPosition = currentPosition;
    }

    // 1. Сохраняем информацию о выделенных машинах
    std::set<int> highlightedMachines;  // Набор для хранения выделенных машин
    highlightedMachines.clear();  // Очищаем набор выделенных машин
    std::set<int> highlightedJobs;  // Набор для хранения выделенных деталей
    highlightedJobs.clear();  // Очищаем набор выделенных деталей
    for (auto &sOp : m_vMsOperations_cont) {
        if (sOp.bHighlighted) {
            highlightedMachines.insert(sOp.iMachine);  // Добавляем выделенную машину в набор
            highlightedJobs.insert(sOp.iJob);  // Добавляем выделенную деталь в набор
        }
    }

    // 2. Отрисовка подписей для машин
    for (int i = 0; i < fMachineRowCount; ++i) {
        int iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
        pPainter->setPen(gridPen);
        pPainter->drawLine(ioffsetFromSide, iYPos, iScreenWidth - ioffsetFromSide, iYPos);

        // Проверяем, выделена ли текущая машина
        if (highlightedMachines.count(i + 1)) {
            QFont boldFont = pPainter->font();
            boldFont.setBold(true);  // Устанавливаем жирный шрифт
            pPainter->setFont(boldFont);
            pPainter->setPen(Qt::black);  // Устанавливаем черный цвет текста

//            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1)) - 6, iYPos - pPainter->fontMetrics().height() / 2 - 2, pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1)) + 8, pPainter->fontMetrics().height() + 4);
            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1))-2 , iYPos - pPainter->fontMetrics().height() / 2 , pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1)) , pPainter->fontMetrics().height() );

            pPainter->setBrush(QBrush(Qt::yellow));  // Fill rectangle with yellow
            pPainter->setPen(Qt::yellow);
            pPainter->drawRect(rect);
            pPainter->setBrush(Qt::NoBrush);  // Reset brush
            pPainter->setPen(Qt::black);  // Set black text color for label


        } else {
            pPainter->setFont(verticalFont);  // Стандартный шрифт
            pPainter->setPen(textPen);  // Стандартный цвет текста
        }

        // Отрисовка подписи
        pPainter->drawText(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1))-1.5, iYPos + 5, QString("Р %1").arg(i + 1));
    }

    // 2. Отрисовка подписей для деталей
    for (int i = 0; i < fJobRowCount; ++i) {
        int iYPos = iOffsetYMs + (i + 0.5) * iMachineHeight;
        pPainter->setPen(gridPen);
        pPainter->drawLine(ioffsetFromSide, iYPos, iScreenWidth - ioffsetFromSide, iYPos);

        // Проверяем, выделена ли текущая деталь
        if (highlightedJobs.count(i + 1)) {
            QFont boldFont = pPainter->font();
            boldFont.setBold(true);  // Устанавливаем жирный шрифт
            pPainter->setFont(boldFont);
            pPainter->setPen(Qt::black);  // Устанавливаем черный цвет текста

            // Draw yellow rectangle around the label to highlight it
//            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)) - 6, iYPos - pPainter->fontMetrics().height() / 2 - 2, pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)) + 8, pPainter->fontMetrics().height() + 4);

            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)) - 2, iYPos - pPainter->fontMetrics().height() / 2 , pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)) , pPainter->fontMetrics().height() );
            pPainter->setBrush(QBrush(Qt::yellow));  // Fill rectangle with yellow
            pPainter->setPen(Qt::yellow);
            pPainter->drawRect(rect);
            pPainter->setBrush(Qt::NoBrush);  // Reset brush
            pPainter->setPen(Qt::black);  // Set black text color for label
        } else {
            pPainter->setFont(verticalFont);  // Стандартный шрифт
            pPainter->setPen(textPen);  // Стандартный цвет текста
        }

        // Отрисовка подписи
        pPainter->drawText(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1))-1.5, iYPos + 5, QString("Д %1").arg(i + 1));

    }

    pPainter->setPen(gridPen);
    // Create a border for the top Gantt chart (machine chart)
    pPainter->drawLine(ioffsetFromSide, iOffsetYJs, iScreenWidth - ioffsetFromSide, iOffsetYJs);  // Top border
    pPainter->drawLine(iScreenWidth - ioffsetFromSide, iOffsetYJs, iScreenWidth - ioffsetFromSide, iYPos_workers);  // Right border
    // Create a border for the bottom Gantt chart (job chart)
    pPainter->drawLine(ioffsetFromSide, iOffsetYMs, iScreenWidth - ioffsetFromSide, iOffsetYMs);  // Top border for the job chart
    pPainter->drawLine(iScreenWidth - ioffsetFromSide, iOffsetYMs, iScreenWidth - ioffsetFromSide, iYPos_details);  // Right border for the job chart

    int distance = ioffsetFromSide / 2;  // Например, 1/3 от ширины текста

    QFontMetrics oMachineMetrics(pPainter->font());
    int iMachineTextHeight = oMachineMetrics.height();

    // Вертикальная подпись "Рабочие"
    pPainter->setFont(verticalFontRotate);
    pPainter->setPen(textPen);

    pPainter->save();
    int iMachineCenterY = (iOffsetYJs + iOffsetYJs + fMachineRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2 + distance / 2;
    pPainter->translate(ioffsetFromSide * 0.3 , iMachineCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Рабочие");
    pPainter->restore();  // Восстанавливаем исходную систему координат

    // Вертикальная подпись "Детали"
    pPainter->setFont(verticalFontRotate);
    pPainter->setPen(textPen);

    pPainter->save();
    int iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2 + distance / 2;
    pPainter->translate(ioffsetFromSide * 0.3 , iJobCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Детали");
    pPainter->restore();  // Восстанавливаем исходную систему координат


    // Отрисовка баров для операций на графике машин (m_vMsOperations_cont)
    for (auto &sOp : m_vMsOperations_cont) {
        int iBarStartX = ioffsetFromSide + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight + iMachineHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки машины
        QRect oMachineRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

        // Сохраняем прямоугольник в структуре операции
        sOp.rect = oMachineRect;

        // Если бар выделен, изменяем цвет и толщину пера
        if (sOp.bHighlighted) {
            pPainter->setPen(QPen(Qt::black, 3)); // Черный жирный контур
            pPainter->fillRect(oMachineRect, Qt::yellow); // Желтая заливка
        } else {
            pPainter->setPen(textPen);
            pPainter->fillRect(oMachineRect, m_umapJobColors[sOp.iJob]);
        }


        if (sOp.bHighlighted) {
            pPainter->drawRect(oMachineRect);
            pPainter->setFont(dynamicFont);
            dynamicFont.setBold(true);  // Делаем шрифт жирным
            pPainter->setPen(Qt::black); // Черный цвет текста
        }
        else {
            pPainter->drawRect(oMachineRect);
            pPainter->setFont(dynamicFont);
        }

//        pPainter->drawRect(oMachineRect);
//        pPainter->setFont(dynamicFont);

        // Формируем текст для подписи
        QString labelText = QString("Д %1").arg(sOp.iJob);
        pPainter->drawText(oMachineRect, Qt::AlignCenter, labelText);
    }

    // Отрисовка баров для операций на графике задач (m_vJsOperations_cont)
    for (auto &sOp : m_vJsOperations_cont) {
        int iBarStartX = ioffsetFromSide + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iMachineHeight + iMachineHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки задачи
        QRect oJobRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

        // Сохраняем прямоугольник в структуре операции
        sOp.rect = oJobRect;

        // Если бар выделен, изменяем цвет и толщину пера
        if (sOp.bHighlighted) {
            pPainter->setPen(QPen(Qt::black, 3)); // Черный жирный контур
            pPainter->fillRect(oJobRect, Qt::yellow); // Желтая заливка

        } else {
            pPainter->setPen(textPen);
            pPainter->fillRect(oJobRect, m_umapJobColors[sOp.iJob]);
            // обычный шрифт для жирной подписи по оси y

        }

        if (sOp.bHighlighted) {
            pPainter->drawRect(oJobRect);
            pPainter->setFont(dynamicFont);
            dynamicFont.setBold(true);  // Делаем шрифт жирным
            pPainter->setPen(Qt::black); // Черный цвет текста
        }
        else {
            pPainter->drawRect(oJobRect);
            pPainter->setFont(dynamicFont);
        }


        // Формируем текст для подписи с машинами
        QString labelText = "Р ";
        for (size_t i = 0; i < sOp.vMachinesIndexes.size(); ++i) {
            labelText += QString::number(sOp.vMachinesIndexes[i]);
            if (i != sOp.vMachinesIndexes.size() - 1) {
                labelText += ", ";  // Добавляем запятую между номерами машин, кроме последнего
            }
        }

        pPainter->drawText(oJobRect, Qt::AlignCenter, labelText);
    }

}


void GanttChart::DrawWorkersTimeChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight) {
    if (!pPainter->isActive()) {
        qWarning("QPainter is not active");
        return;
    }

    // Собираем данные о времени работы каждого рабочего
    QMap<int, int> qmworkerTime;
    QMap<int, int> qmworkerJobQount;
    for (const auto &op : m_vMsOperations_cont) {
        qmworkerTime[op.iMachine] += (op.iFinish - op.iStart);
        if (op.iJob) {
            qmworkerJobQount[op.iMachine] += 1;
        }
    }

    // Определение максимальных значений для масштабирования графика
    int maxTime = *std::max_element(qmworkerTime.begin(), qmworkerTime.end());
    int maxJobCount = *std::max_element(qmworkerJobQount.begin(), qmworkerJobQount.end());

    if (maxTime == 0) {
        return; // Если нет данных, выход
    }

    int numWorkers = qmworkerTime.size();
    int margin = std::min(iScreenWidth, iScreenHeight) * 0.01;
    // Ширина для каждого бара
    int barWidth = (iScreenWidth - 20 * margin) / (numWorkers * 1.6);

    // Масштабные коэффициенты и отступы
    int marginHorizontal = iScreenWidth * 0.1;
    int marginVertical = iScreenHeight * 0.1;
    int labelOffset = iScreenHeight * 0.05;

    // Динамический размер шрифта
    int fontSize = std::min(iScreenWidth, iScreenHeight) * 0.02;
    QFont font = pPainter->font();
    font.setPointSize(fontSize);
    pPainter->setFont(font);

    int xAxisHeight = iScreenHeight * 0.15;
    int graphTop = marginVertical + labelOffset;
    int graphBottom = iScreenHeight - xAxisHeight;
    int graphHeight = graphBottom - graphTop;
    int graphWidth = barWidth * numWorkers + 0.5 * barWidth + marginHorizontal * 0.2 * (numWorkers - 1);  // Скорректированная ширина оси X

    QString maxTimeLabel = QString::number(maxTime);
    QString maxWorkerLabel = QString("Р%1").arg(numWorkers);

    while ((graphWidth + 2 * marginHorizontal) > iScreenWidth && barWidth > 1) {
        barWidth -= 1;
        graphWidth = barWidth * numWorkers + 0.5 * barWidth + marginHorizontal * 0.2 * (numWorkers - 1);
    }


    // Отрисовка осей
    pPainter->drawLine(marginHorizontal, graphTop, marginHorizontal, graphBottom); // Ось Y слева
    pPainter->drawLine(marginHorizontal, graphBottom, marginHorizontal + graphWidth, graphBottom); // Ось X
    pPainter->drawLine(marginHorizontal + graphWidth , graphTop, marginHorizontal + graphWidth , graphBottom); // Вторая ось Y справа сдвинута на barWidth правее последней диаграммы

    // Подпись графика сверху
    pPainter->drawText(marginHorizontal, marginVertical / 2, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Время по рабочим");
//    pPainter->drawText(marginHorizontal, marginVertical + labelOffset, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Время по рабочим");

    // Подписи по первой оси Y (Время в минутах)
    for (int i = 0; i <= maxTime; i += (maxTime / 10)) {
        int y = graphBottom - (graphHeight * i / maxTime);
//        pPainter->drawText(0, y - fontSize / 2, marginHorizontal - 10, fontSize * 2, Qt::AlignRight, QString::number(i));
        pPainter->drawText(0, y - fontSize / 2, marginHorizontal - marginHorizontal/10, fontSize * 2, Qt::AlignRight, QString::number(i));
    }

    // Подпись по второй оси Y (Количество операций)
    if (maxJobCount > 0) {  // Проверка, чтобы избежать деления на ноль
        for (int i = 0; i <= maxJobCount; i += std::max(1, maxJobCount / 10)) {  // Используем std::max, чтобы шаг был не меньше 1
            int y = graphBottom - (graphHeight * i / maxJobCount);
//            pPainter->drawText(marginHorizontal + graphWidth + 10, y - fontSize / 2, marginHorizontal - 10, fontSize * 2, Qt::AlignLeft, QString::number(i));
            pPainter->drawText(marginHorizontal + graphWidth + marginHorizontal/10, y - fontSize / 2, marginHorizontal - marginHorizontal/10, fontSize * 2, Qt::AlignLeft, QString::number(i));

        }
    }

    // Поворот и подпись "Время (мин)" для оси Y
    int textWidth = pPainter->fontMetrics().horizontalAdvance("Время (мин)");
    int textHeight = pPainter->fontMetrics().height();
    pPainter->save();
    int textVerticalPosition = (iScreenHeight / 2) + textWidth / 2;
//    int horizontalMargin = iScreenWidth * 0.01;
//    int horizontalMargin = marginHorizontal * 0.1;

    pPainter->translate(marginHorizontal*0.1, textVerticalPosition);
    pPainter->rotate(-90);
//    pPainter->drawText(0, 0, textWidth, textHeight, Qt::AlignCenter, "Время (мин)");
    pPainter->drawText(0, 0, textWidth, textHeight, Qt::AlignLeft, "Время (мин)");
    pPainter->restore();

    // Подпись "Количество работ" для правой оси Y с отступом
    int rightAxisTextWidth = pPainter->fontMetrics().horizontalAdvance("Количество работ");
    int rightAxisTextHeight = pPainter->fontMetrics().height();
    pPainter->save();
//    pPainter->translate(marginHorizontal + graphWidth + 2 * barWidth + marginHorizontal * 0.5, textVerticalPosition);
//    pPainter->translate(marginHorizontal*1.1 + graphWidth + barWidth , textVerticalPosition);
//    pPainter->translate(marginHorizontal + graphWidth + marginHorizontal/2, textVerticalPosition);
    pPainter->translate(iScreenWidth- marginHorizontal*0.2 , textVerticalPosition);

    pPainter->rotate(-90);
//    pPainter->drawText(0, -rightAxisTextHeight / 2, rightAxisTextWidth, rightAxisTextHeight, Qt::AlignCenter, "Количество работ");
    pPainter->drawText(0, -rightAxisTextHeight / 2, rightAxisTextWidth, rightAxisTextHeight, Qt::AlignCenter, "Количество работ");

    pPainter->restore();

    // Подпись "Номер рабочего" под осью X
    pPainter->drawText(marginHorizontal, iScreenHeight - labelOffset, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Номер рабочего");

    // Отрисовка баров
    int xPosition = marginHorizontal;
    for (auto i = qmworkerTime.begin(); i != qmworkerTime.end(); ++i) {
        int machineId = i.key();
        int time = i.value();
        int jobCount = qmworkerJobQount[machineId];

        int barHeightTime = graphHeight * time / maxTime;
        int barHeightJob = graphHeight * jobCount / maxJobCount;

        // Рисуем синий бар для времени
        QString label = QString("Р%1").arg(machineId);
        pPainter->drawText(xPosition, graphBottom + labelOffset / 2, barWidth, labelOffset, Qt::AlignCenter, label);
        pPainter->fillRect(xPosition, graphBottom - barHeightTime, barWidth / 2 - 2, barHeightTime, Qt::blue);

        // Отображение значения времени над синим баром
        if (time > 0) {
            int textHeight = pPainter->fontMetrics().height() ;  // Уменьшаем высоту текста в 2 раза
            int textWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(time)); // Определяем ширину текста
            int yPosition = graphBottom - barHeightTime - marginVertical*0.5;  // Поднимаем текст немного выше
            // Проверяем, помещается ли текст в текущую ширину бара, и уменьшаем шрифт при необходимости
            if (textWidth > (barWidth / 2 - 2)) {
                QFont smallerFont = pPainter->font();
                smallerFont.setPointSize(smallerFont.pointSize() - 2); // Уменьшаем шрифт на 2 пункта
                pPainter->setFont(smallerFont);
            }

            pPainter->drawText(xPosition, yPosition + marginVertical * 0.1, barWidth / 2 - 2, textHeight, Qt::AlignCenter, QString::number(time));
        }


        // Рисуем красный бар для количества операций рядом, с делением на части
        int segmentHeight = barHeightJob / std::max(1, jobCount);
        for (int j = 0; j < jobCount; ++j) {
            int segmentTop = graphBottom - barHeightJob + j * segmentHeight;
            pPainter->fillRect(xPosition + barWidth / 2 + 2, segmentTop, barWidth / 2 - 2, segmentHeight - 2, Qt::red);
        }

        xPosition += barWidth + marginHorizontal * 0.2; // Увеличиваем xPosition, добавляя 4 пикселя между барами
    }
}


//void GanttChart::DrawWorkersTimeChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight) {
//    if (!pPainter->isActive()) {
//        qWarning("QPainter is not active");
//        return;
//    }

//    // Собираем данные о времени работы каждого рабочего
//    QMap<int, int> qmworkerTime;
//    QMap<int, int> qmworkerJobQount;
//    for (const auto &op : m_vMsOperations_cont) {
//        qmworkerTime[op.iMachine] += (op.iFinish - op.iStart);
//        if (op.iJob) {
//            qmworkerJobQount[op.iMachine] += 1;
//        }
//    }

//    // Определение максимальных значений для масштабирования графика
//    int maxTime = *std::max_element(qmworkerTime.begin(), qmworkerTime.end());
//    int maxJobCount = *std::max_element(qmworkerJobQount.begin(), qmworkerJobQount.end());

//    if (maxTime == 0) {
//        return; // Если нет данных, выход
//    }

//    int numWorkers = qmworkerTime.size();
//    int margin = std::min(iScreenWidth, iScreenHeight) * 0.01;
//    // Ширина для каждого бара
//    int barWidth = (iScreenWidth - 20 * margin) / (numWorkers * 1.6);

//    // Масштабные коэффициенты и отступы
//    int marginHorizontal = iScreenWidth * 0.1;
//    int marginVertical = iScreenHeight * 0.1;
//    int labelOffset = iScreenHeight * 0.05;

//    // Динамический размер шрифта
//    int fontSize = std::min(iScreenWidth, iScreenHeight) * 0.02;
//    QFont font = pPainter->font();
//    font.setPointSize(fontSize);
//    pPainter->setFont(font);

//    int xAxisHeight = iScreenHeight * 0.15;
//    int graphTop = marginVertical + labelOffset;
//    int graphBottom = iScreenHeight - xAxisHeight;
//    int graphHeight = graphBottom - graphTop;
//    int graphWidth = barWidth * numWorkers + 0.5 * barWidth + marginHorizontal * 0.2 * (numWorkers - 1);  // Скорректированная ширина оси X

//    // Отрисовка осей
//    pPainter->drawLine(marginHorizontal, graphTop, marginHorizontal, graphBottom); // Ось Y слева
//    pPainter->drawLine(marginHorizontal, graphBottom, marginHorizontal + graphWidth, graphBottom); // Ось X
//    pPainter->drawLine(marginHorizontal + graphWidth , graphTop, marginHorizontal + graphWidth , graphBottom); // Вторая ось Y справа сдвинута на barWidth правее последней диаграммы

//    // Подпись графика сверху
//    pPainter->drawText(marginHorizontal, marginVertical / 2, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Время по рабочим");

//    // Подписи по первой оси Y (Время в минутах)
//    for (int i = 0; i <= maxTime; i += (maxTime / 10)) {
//        int y = graphBottom - (graphHeight * i / maxTime);
//        pPainter->drawText(0, y - fontSize / 2, marginHorizontal - 10, fontSize * 2, Qt::AlignRight, QString::number(i));
//    }

//    // Подпись по второй оси Y (Количество операций)
//    if (maxJobCount > 0) {  // Проверка, чтобы избежать деления на ноль
//        for (int i = 0; i <= maxJobCount; i += std::max(1, maxJobCount / 10)) {  // Используем std::max, чтобы шаг был не меньше 1
//            int y = graphBottom - (graphHeight * i / maxJobCount);
//            pPainter->drawText(marginHorizontal + graphWidth + 10, y - fontSize / 2, marginHorizontal - 10, fontSize * 2, Qt::AlignLeft, QString::number(i));
//        }
//    }

//    // Поворот и подпись "Время (мин)" для оси Y
//    int textWidth = pPainter->fontMetrics().horizontalAdvance("Время (мин)");
//    int textHeight = pPainter->fontMetrics().height();
//    pPainter->save();
//    int textVerticalPosition = (iScreenHeight / 2) + textWidth / 2;
//    int horizontalMargin = iScreenWidth * 0.01;

//    pPainter->translate(horizontalMargin, textVerticalPosition);
//    pPainter->rotate(-90);
//    pPainter->drawText(0, 0, textWidth, textHeight, Qt::AlignCenter, "Время (мин)");
//    pPainter->restore();

//    // Подпись "Количество работ" для правой оси Y с отступом
//    int rightAxisTextWidth = pPainter->fontMetrics().horizontalAdvance("Количество работ");
//    int rightAxisTextHeight = pPainter->fontMetrics().height();
//    pPainter->save();
////    pPainter->translate(marginHorizontal + graphWidth + 2 * barWidth + marginHorizontal * 0.5, textVerticalPosition);
//    pPainter->translate(marginHorizontal*1.1 + graphWidth + barWidth , textVerticalPosition);
//    pPainter->rotate(-90);
//    pPainter->drawText(0, -rightAxisTextHeight / 2, rightAxisTextWidth, rightAxisTextHeight, Qt::AlignCenter, "Количество работ");
//    pPainter->restore();

//    // Подпись "Номер рабочего" под осью X
//    pPainter->drawText(marginHorizontal, iScreenHeight - labelOffset, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Номер рабочего");

//    // Отрисовка баров
//    int xPosition = marginHorizontal;
//    for (auto i = qmworkerTime.begin(); i != qmworkerTime.end(); ++i) {
//        int machineId = i.key();
//        int time = i.value();
//        int jobCount = qmworkerJobQount[machineId];

//        int barHeightTime = graphHeight * time / maxTime;
//        int barHeightJob = graphHeight * jobCount / maxJobCount;

//        // Рисуем синий бар для времени
//        QString label = QString("Р%1").arg(machineId);
//        pPainter->drawText(xPosition, graphBottom + labelOffset / 2, barWidth, labelOffset, Qt::AlignCenter, label);
//        pPainter->fillRect(xPosition, graphBottom - barHeightTime, barWidth / 2 - 2, barHeightTime, Qt::blue);

//        // Отображение значения времени над синим баром
//        if (time > 0) {
//            int textHeight = pPainter->fontMetrics().height() ;  // Уменьшаем высоту текста в 2 раза
//            int textWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(time)); // Определяем ширину текста
//            int yPosition = graphBottom - barHeightTime - marginVertical*0.5;  // Поднимаем текст немного выше
//            // Проверяем, помещается ли текст в текущую ширину бара, и уменьшаем шрифт при необходимости
//            if (textWidth > (barWidth / 2 - 2)) {
//                QFont smallerFont = pPainter->font();
//                smallerFont.setPointSize(smallerFont.pointSize() - 2); // Уменьшаем шрифт на 2 пункта
//                pPainter->setFont(smallerFont);
//            }

//            pPainter->drawText(xPosition, yPosition + marginVertical * 0.1, barWidth / 2 - 2, textHeight, Qt::AlignCenter, QString::number(time));
//        }


//        // Рисуем красный бар для количества операций рядом, с делением на части
//        int segmentHeight = barHeightJob / std::max(1, jobCount);
//        for (int j = 0; j < jobCount; ++j) {
//            int segmentTop = graphBottom - barHeightJob + j * segmentHeight;
//            pPainter->fillRect(xPosition + barWidth / 2 + 2, segmentTop, barWidth / 2 - 2, segmentHeight - 2, Qt::red);
//        }

//        xPosition += barWidth + marginHorizontal * 0.2; // Увеличиваем xPosition, добавляя 4 пикселя между барами
//    }
//}
