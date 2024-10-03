#include "GanttChart.h"
#include "JsonReader.h"
#include <random>
#include <QDebug>
#include <iostream>
#include <QMap>
#include <tuple>

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

    // Набор основных цветов, которые будут сильно отличаться друг от друга
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

    // Уменьшение яркости основных цветов на 10%
    for (auto &color : primaryColors) {
        color = color.darker(130);  // 110% от исходной яркости, уменьшение на 10%
//        color.setAlpha(154);        // Установить прозрачность на уровне 80% (204 из 255)

    }
    // Счетчик для отслеживания, когда закончатся основные цвета
    int primaryColorIndex = 0;

    // Генератор для случайных полутонов
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // Создание уникальных цветов для каждого job
    for (const auto &sOp : m_vJsOperations_cont) {
        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
            if (primaryColorIndex < primaryColors.size()) {
                // Если основные цвета еще остались
                m_umapJobColors[sOp.iJob] = primaryColors[primaryColorIndex];
                primaryColorIndex++;
            } else {
                // Если основные цвета закончились, генерируем случайные полутона
                m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
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

//    for (int i = 0; i <= iMaxFinish; i += 10) {
//        pPainter->setPen(gridPen);
//        pPainter->drawLine(ioffsetFromSide + i * iScaleFactorX, iOffsetYJs, ioffsetFromSide + i * iScaleFactorX, iYPos_workers);
//        pPainter->drawLine(ioffsetFromSide + i * iScaleFactorX, iOffsetYMs, ioffsetFromSide + i * iScaleFactorX, iYPos_details);
//        pPainter->setPen(textPen);
//        pPainter->drawText(ioffsetFromSide + i * iScaleFactorX - 10, iOffsetYJs + fMachineRowCount * iMachineHeight +ioffsetFromVertical*0.15, QString::number(i));
//        pPainter->drawText(ioffsetFromSide + i * iScaleFactorX - 10, iOffsetYMs + fJobRowCount * iMachineHeight + ioffsetFromVertical*0.15 , QString::number(i));
//    }

    int lastDrawnPosition = -1;  // Переменная для хранения последней позиции, на которой был нарисован текст
    for (int i = 0; i <= iMaxFinish; i += 10) {
        int currentPosition = ioffsetFromSide + i * iScaleFactorX;

        // Рассчитываем ширину текста для текущего значения
        int textWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(i));

        // Если разница между текущей позицией и последней позицией меньше ширины текста, пропускаем отрисовку
        if (lastDrawnPosition != -1 && (currentPosition - lastDrawnPosition) < textWidth) {
            i += 10;  // Пропускаем одну метку
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


    for (int i = 0; i < fMachineRowCount; ++i) {
        int iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
        pPainter->setPen(gridPen);
        pPainter->drawLine(ioffsetFromSide, iYPos, iScreenWidth - ioffsetFromSide, iYPos);
        pPainter->setPen(textPen);
        pPainter->drawText(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1)), iYPos + 5, QString("Р %1").arg(i + 1));
    }

    for (int i = 0; i < fJobRowCount; ++i) {
        int iYPos = iOffsetYMs + (i + 0.5) * iMachineHeight;
        pPainter->setPen(gridPen);
        pPainter->drawLine(ioffsetFromSide, iYPos, iScreenWidth - ioffsetFromSide, iYPos);
        pPainter->setPen(textPen);
        pPainter->drawText(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)), iYPos + 5, QString("Д %1").arg(i + 1));

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
    pPainter->translate(ioffsetFromSide * 0.5 , iMachineCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Рабочие");
    pPainter->restore();  // Восстанавливаем исходную систему координат

    // Вертикальная подпись "Детали"
    pPainter->setFont(verticalFontRotate);
    pPainter->setPen(textPen);

    pPainter->save();
    int iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2 + distance / 2;
    pPainter->translate(ioffsetFromSide * 0.5 , iJobCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Детали");
    pPainter->restore();  // Восстанавливаем исходную систему координат

    // Отрисовка баров для операций на графике машин (m_vMsOperations_cont)
    for (const auto &sOp : m_vMsOperations_cont) {
        int iBarStartX = ioffsetFromSide + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight + iMachineHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки машины
        QRect oMachineRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

        // Уменьшаем ширину и отрисовываем бар
        pPainter->fillRect(oMachineRect, m_umapJobColors[sOp.iJob]);
        pPainter->setPen(textPen);
        pPainter->drawRect(oMachineRect);
        // Устанавливаем шрифт на основе высоты окна
        pPainter->setFont(dynamicFont);

        // Формируем текст для подписи
        QString labelText;
        labelText = QString("Д %1").arg(sOp.iJob);  // Полная подпись для машины
        // Отрисовка текста
        pPainter->drawText(oMachineRect, Qt::AlignCenter, labelText);
    }

    // Отрисовка баров для операций на графике задач (m_vJsOperations_cont)
    for (const auto &sOp : m_vJsOperations_cont) {
        int iBarStartX = ioffsetFromSide + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iMachineHeight + iMachineHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки задачи
        QRect oJobRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

        // Уменьшаем ширину и отрисовываем бар
        pPainter->fillRect(oJobRect, m_umapJobColors[sOp.iJob]);
        pPainter->setPen(textPen);
        pPainter->drawRect(oJobRect);
        pPainter->setFont(dynamicFont);

        // Формируем текст для подписи с машинами
        QString labelText = "Р ";
        for (size_t i = 0; i < sOp.vMachinesIndexes.size(); ++i) {
            labelText += QString::number(sOp.vMachinesIndexes[i]);
            if (i != sOp.vMachinesIndexes.size() - 1) {
                labelText += ", ";  // Добавляем запятую между номерами машин, кроме последнего
            }
        }
        // Отрисовка текста
        pPainter->drawText(oJobRect, Qt::AlignCenter, labelText);
    }
}

//void GanttChart::DrawGanttChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight) {
//    // Рассчитываем максимальное значение Finish для ограничения оси X
//    int iMaxFinish = 0;
//    for (const auto &op : m_vJsOperations_cont) {
//        if (op.iFinish > iMaxFinish) {
//            iMaxFinish = op.iFinish;
//        }
//    }
//    for (const auto &op : m_vMsOperations_cont) {
//        if (op.iFinish > iMaxFinish) {
//            iMaxFinish = op.iFinish;
//        }
//    }
//    if (iMaxFinish == 0) {
//        iMaxFinish = 1;
//    }

//    // Определение максимального значения machine для верхнего графика и job для нижнего графика
//    int iMaxMachine = 0;
//    int iMaxJob = 0;
//    for (const auto &op : m_vJsOperations_cont) {
//        for (const auto &machineIndex : op.vMachinesIndexes) {
//            if (machineIndex > iMaxMachine) {
//                iMaxMachine = machineIndex;
//            }
//        }
//    }

//    for (const auto &op : m_vMsOperations_cont) {
//        if (op.iJob > iMaxJob) {
//            iMaxJob = op.iJob;
//        }
//    }

//    for (const auto &op : m_vMsOperations_cont) {
//        if (op.iJob > iMaxJob) {
//            iMaxJob = op.iJob;
//        }
//    }

//    float fMachineRowCount = iMaxMachine ;  // Количество строк для машин
//    float fJobRowCount = iMaxJob ;          // Количество строк для задач

//    // Перерисовываем изображение с новыми размерами окна
//    QImage m_oChartImage = QImage(iScreenWidth, iScreenHeight, QImage::Format_ARGB32);
//    m_oChartImage.fill(Qt::white);

//    pPainter->setPen(QPen(Qt::black, 2));

//    QFontMetrics oMetrics(pPainter->font());
//    int iLabelWidth = oMetrics.horizontalAdvance("Рабочие 10000");
//    int iLabelWidthWithPadding = static_cast<int>(iLabelWidth * 1.2);

//    int iOffset1 = std::max(static_cast<int>(iScreenWidth * 0.05), 50);
//    int iOffset3 = std::max(static_cast<int>(iScreenWidth * 0.015), 10);

//    int iMinOffset2 = static_cast<int>(iLabelWidthWithPadding * 0.5);
//    int iMaxOffset2 = iLabelWidthWithPadding;
//    int iOffset2 = std::min(std::max(static_cast<int>(iScreenWidth * 0.2), iMinOffset2), iMaxOffset2);

//    int iLabelBoundX = iOffset1 + iOffset2;
//    int iLabelOffsetX = iLabelBoundX + iOffset3;
//    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - iOffset1) / iMaxFinish;


//    // Определение отступов от высоты экрана
//    int offsetToXAxisLabels = iScreenHeight * 0.05;  // Отступ от оси X до подписей значений по оси X
//    int offsetToTimeLabel = iScreenHeight * 0.03;    // Отступ от подписей значений по оси X до подписи "Время (мин)"
//    int offsetToBottomEdge = iScreenHeight * 0.04;   // Отступ от подписи "Время (мин)" до нижнего края экрана или до подписи "Диаграмма по деталям"

//    // Вычисляем отступы в процентном соотношении от высоты экрана
//    int iMinTopOffset = static_cast<int>(iScreenHeight * 0.02);  // 2% от высоты экрана
//    int iMinBottomOffset = static_cast<int>(iScreenHeight * 0.05);  // 5% от высоты экрана

//    // Расчет доступной высоты для графиков, учитывая новый отступ снизу
//    int availableHeight = iScreenHeight - iScreenHeight * 0.1 - offsetToBottomEdge*2 - offsetToTimeLabel-offsetToXAxisLabels -iMinBottomOffset;

////    int iMachineHeight = availableHeight * 0.07;
////    int iJobHeight = availableHeight * 0.07;

//    float totalRowCount = fMachineRowCount + fJobRowCount;
//    float spacingFactor = 1.1;  // С учетом 10% отступа между барами

//    // Расчет процентной доли от availableHeight для минимальной и максимальной высоты бара
//    float minBarHeightPercentage = 0.01; // 1% от availableHeight
//    float maxBarHeightPercentage = 0.10; // 10% от availableHeight

//    int minBarHeight = static_cast<int>(availableHeight * minBarHeightPercentage);
//    int maxBarHeight = static_cast<int>(availableHeight * maxBarHeightPercentage);

//    // Вычисляем начальное значение высоты бара, основываясь на общем количестве строк и доступной высоте
//    int iMachineHeight = static_cast<int>(availableHeight / (totalRowCount * spacingFactor));
//    // Корректируем высоту баров, чтобы она не выходила за установленные процентные пределы
//    iMachineHeight = std::max(minBarHeight, std::min(maxBarHeight, iMachineHeight));


//    int iOffsetYJs = std::max(static_cast<int>(iScreenHeight * 0.1), iMinTopOffset);
////    int iOffsetYMs = iMachineHeight*2 + fMachineRowCount * iMachineHeight + std::max(static_cast<int>(iScreenHeight * 0.05), iMinTopOffset)+offsetToXAxisLabels;
//    // Используем iMinBottomOffset для расчета позиции нижнего графика
//    int iOffsetYMs = iMachineHeight * 2 + fMachineRowCount * iMachineHeight + std::max(static_cast<int>(iScreenHeight * 0.05), iMinTopOffset) + offsetToXAxisLabels + iMinBottomOffset;

//    // Расчет высоты, доступной для графиков, учитывая новый отступ снизу
//    // Учет отступов для подписей
//    int labelPadding = static_cast<int>(iScreenHeight * 0.02);
//    int iBottomOffset = static_cast<int>(iScreenHeight * 0.1); // 10% от нижнего края экрана

//    // Подписи графиков
////    pPainter->setPen(QPen(Qt::black, 1));

//    int midPointMachine = iLabelOffsetX + (iScreenWidth - iLabelOffsetX) / 2 - oMetrics.horizontalAdvance("Диаграмма по рабочим") / 2;
//    int midPointJob = iLabelOffsetX + (iScreenWidth - iLabelOffsetX) / 2 - oMetrics.horizontalAdvance("Диаграмма по деталям") / 2;

//    int iFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.015));
//    QFont oFont = pPainter->font();
//    oFont.setPointSize(iFontSize);
//    pPainter->setFont(oFont);

//    oFont.setBold(true); // Установка жирного шрифта
//    pPainter->setPen(QPen(Qt::black, 2)); // Жирный и черный контур текста
//    // Отрисовка заголовков графиков
//    pPainter->drawText(midPointMachine, iOffsetYJs - iScreenHeight * 0.01, "Диаграмма по рабочим");
//    pPainter->drawText(midPointJob, iOffsetYMs - iScreenHeight * 0.01, "Диаграмма по деталям");


//    QPen oGridPen(QColor(0, 0, 0, 50));
//    pPainter->setPen(oGridPen);

//    qDebug() << "iOffsetYJs:" << iOffsetYJs << "iOffsetYMs:" << iOffsetYMs;
//    qDebug() << "fMachineRowCount:" << fMachineRowCount << "fJobRowCount:" << fJobRowCount;

////    // Центрирование подписи "Время (мин)" между графиками
//    int timeLabelX = iLabelOffsetX + (iScreenWidth - iLabelOffsetX) / 2 - iLabelWidth / 2;
//    int labelOffset = static_cast<int>(iScreenHeight * 0.04); // Отступ для "Время (мин)" пропорционально высоте экрана

//    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
//    pPainter->drawText(timeLabelX, iOffsetYJs + fMachineRowCount * iMachineHeight + labelOffset, "Время (мин)");
////    pPainter->drawText(timeLabelX, iOffsetYMs + fJobRowCount * iMachineHeight + labelOffset, "Время (мин)");


//    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
//    int labelPositionY = iOffsetYMs + fJobRowCount * iMachineHeight + labelOffset;
//    int availableSpaceForLabel = iScreenHeight - labelPositionY;

//    // Проверяем, достаточно ли места для отступа снизу
//    if (availableSpaceForLabel < iMinBottomOffset) {
//        labelPositionY = iScreenHeight - iMinBottomOffset;  // Поднимаем подпись вверх, чтобы обеспечить отступ снизу
//    }

//    pPainter->drawText(timeLabelX, labelPositionY, "Время (мин)");

//    int timeLabelYBottom =iOffsetYMs + fJobRowCount * iMachineHeight + labelOffset;
//    // Убедимся, что подпись не заезжает на график
//    if (timeLabelYBottom > iScreenHeight - 10) { // Если подпись слишком низко, поднимаем выше
//        timeLabelYBottom = iScreenHeight - 10;
//        pPainter->drawText(timeLabelX, timeLabelYBottom, "Время (мин)");
//    }

//     // рассчитываю где ось x для верхнего графика
//    int iYPos_workers = iOffsetYJs + (fMachineRowCount-1 + 0.5) * iMachineHeight; // позиция для оси х по рабочим
//    int iYPos_details = iOffsetYMs + (fJobRowCount-1 + 0.5) * iMachineHeight; // позиция для оси х по деталям

//    for (int i = 0; i <= iMaxFinish; i += 10) {
//        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iYPos_workers);
//        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iYPos_details);
////        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + fJobRowCount * iMachineHeight);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + fMachineRowCount * iMachineHeight +iScreenHeight * 0.01, QString::number(i));
//        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + fJobRowCount * iMachineHeight + iScreenHeight * 0.01 , QString::number(i));
//        pPainter->setPen(oGridPen);
//    }

//    for (int i = 0; i < fMachineRowCount; ++i) {
//        int iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
//        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1)), iYPos + 5, QString("Р %1").arg(i + 1));
//        pPainter->setPen(oGridPen);
//    }

//    for (int i = 0; i < fJobRowCount; ++i) {
//        int iYPos = iOffsetYMs + (i + 0.5) * iMachineHeight;
//        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)), iYPos + 5, QString("Д %1").arg(i + 1));
//        pPainter->setPen(oGridPen);
//    }

//    // After the grid and bars are drawn, add this code block to create the border
//    // Adjusting the pen to match the Y-axis pen
//    QPen axisPen = QPen(QColor(0, 0, 0, 50));  // Ensure this matches the Y-axis pen
//    pPainter->setPen(axisPen);

//    // Create a border for the top Gantt chart (machine chart)
//    pPainter->drawLine(iLabelOffsetX, iOffsetYJs, iScreenWidth - iOffset1, iOffsetYJs);  // Top border
//    pPainter->drawLine(iScreenWidth - iOffset1, iOffsetYJs, iScreenWidth - iOffset1, iYPos_workers);  // Right border
////    pPainter->drawLine(iScreenWidth - iOffset1, iOffsetYJs, iScreenWidth - iOffset1, iOffsetYJs + fMachineRowCount * iMachineHeight);  // Right border

//    // Create a border for the bottom Gantt chart (job chart)
//    pPainter->drawLine(iLabelOffsetX, iOffsetYMs, iScreenWidth - iOffset1, iOffsetYMs);  // Top border for the job chart
//    pPainter->drawLine(iScreenWidth - iOffset1, iOffsetYMs, iScreenWidth - iOffset1, iYPos_details);  // Right border for the job chart


//    int distance = iLabelWidth / 2;  // Например, 1/3 от ширины текста

//    QFont oVerticalFont;
////    oVerticalFont.setBold(true); // Установка жирного шрифта
//    oVerticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));

//    // Перерисовываем шрифт для "Рабочие"
//    pPainter->setFont(oVerticalFont);
//    pPainter->setPen(QPen(Qt::black, 2)); // Жирный и черный контур текста

//    pPainter->save();
//    QFontMetrics oMachineMetrics(pPainter->font());
//    int iMachineTextHeight = oMachineMetrics.height();
//    int iMachineCenterY = (iOffsetYJs + iOffsetYJs + fMachineRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2;
////    pPainter->translate(iOffset1 - 10, iMachineCenterY);
//    pPainter->translate(iOffset1 + distance, iMachineCenterY);
//    pPainter->rotate(-90);
//    pPainter->drawText(0, 0, "Рабочие");
//    pPainter->restore();

//    // Перерисовываем шрифт для "Детали"
//    pPainter->setFont(oVerticalFont);

//    pPainter->save();
//    QFontMetrics oJobMetrics(pPainter->font());
//    int iJobTextHeight = oJobMetrics.height();
//    int iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iMachineHeight) / 2 + iJobTextHeight / 2;
////    pPainter->translate(iOffset1 - 10, iJobCenterY);
//    pPainter->translate(iOffset1 + distance, iJobCenterY);
//    pPainter->rotate(-90);
//    pPainter->drawText(0, 0, "Детали");
//    pPainter->restore();

//    // Отрисовка баров для операций на графике машин (m_vMsOperations_cont)
//    for (const auto &sOp : m_vMsOperations_cont) {
//        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
//        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
//        int iBarCenterY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight + iMachineHeight / 2;

//        // Уменьшаем высоту бара до 50% от высоты строки машины
//        QRect oMachineRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

//        // Уменьшаем ширину и отрисовываем бар
//        pPainter->fillRect(oMachineRect, m_umapJobColors[sOp.iJob]);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawRect(oMachineRect);

//        // Устанавливаем шрифт на основе высоты окна
//        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
//        QFont dynamicFont = pPainter->font();
//        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
//        pPainter->setFont(dynamicFont);

//        // Формируем текст для подписи
//        QString labelText;
//        labelText = QString("Д %1").arg(sOp.iJob);  // Полная подпись для машины
//        // Отрисовка текста
//        pPainter->drawText(oMachineRect, Qt::AlignCenter, labelText);
//    }

//    // Отрисовка баров для операций на графике задач (m_vJsOperations_cont)
//    for (const auto &sOp : m_vJsOperations_cont) {
//        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
//        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
//        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iMachineHeight + iMachineHeight / 2;

//        // Уменьшаем высоту бара до 50% от высоты строки задачи
//        QRect oJobRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

//        // Уменьшаем ширину и отрисовываем бар
//        pPainter->fillRect(oJobRect, m_umapJobColors[sOp.iJob]);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawRect(oJobRect);

//        // Устанавливаем шрифт на основе высоты окна
//        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
//        QFont dynamicFont = pPainter->font();
//        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
//        pPainter->setFont(dynamicFont);

//        // Формируем текст для подписи с машинами
//        QString labelText = "Р ";
//        for (size_t i = 0; i < sOp.vMachinesIndexes.size(); ++i) {
//            labelText += QString::number(sOp.vMachinesIndexes[i]);
//            if (i != sOp.vMachinesIndexes.size() - 1) {
//                labelText += ", ";  // Добавляем запятую между номерами машин, кроме последнего
//            }
//        }

//        // Отрисовка текста
//        pPainter->drawText(oJobRect, Qt::AlignCenter, labelText);
//    }

//}

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
        if (op.iJob){
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
    int barWidth = std::min((iScreenWidth - 2 * margin) / (2 * numWorkers + 2), static_cast<int>(iScreenHeight * 0.5));
    int halfBarWidth = barWidth / 2;

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

    // Отрисовка осей
    pPainter->drawLine(marginHorizontal, graphTop, marginHorizontal, graphBottom); // Y axis
    pPainter->drawLine(marginHorizontal, graphBottom, iScreenWidth - marginHorizontal, graphBottom); // X axis
    pPainter->drawLine(iScreenWidth - marginHorizontal, graphTop, iScreenWidth - marginHorizontal, graphBottom); // Вторая ось Y

    // Подпись графика сверху
    pPainter->drawText(marginHorizontal, marginVertical / 2, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Время по рабочим");

    // Подписи по первой оси Y (Время в минутах)
    for (int i = 0; i <= maxTime; i += (maxTime / 10)) {
        int y = graphBottom - (graphHeight * i / maxTime);
        pPainter->drawText(0, y - fontSize / 2, marginHorizontal - 10, fontSize*2, Qt::AlignRight, QString::number(i));
    }

    // Подпись по второй оси Y (Количество операций)
    if (maxJobCount > 0) {  // Добавляем проверку, чтобы избежать деления на ноль
        for (int i = 0; i <= maxJobCount; i += std::max(1, maxJobCount / 10)) {  // Используем std::max, чтобы шаг был не меньше 1
            int y = graphBottom - (graphHeight * i / maxJobCount);
            pPainter->drawText(iScreenWidth - marginHorizontal + 10, y - fontSize / 2, marginHorizontal - 10, fontSize * 2, Qt::AlignLeft, QString::number(i));
        }
    }
//    else {
//        // Если maxJobCount равно 0, можно отобразить какую-то информацию или пропустить отрисовку
//        pPainter->drawText(iScreenWidth - marginHorizontal + 10, graphBottom, marginHorizontal - 10, fontSize * 2, Qt::AlignLeft, "0");

//    }


    // Поворачиваем текст "Время (мин)" на оси Y
    int textWidth = pPainter->fontMetrics().horizontalAdvance("Время (мин)");
    int textHeight = fontSize;
    pPainter->save();
    int textVerticalPosition = (iScreenHeight / 2) + textWidth / 2;
    int horizontalMargin = iScreenWidth * 0.01;

    pPainter->translate(horizontalMargin, textVerticalPosition);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, textWidth, textHeight, Qt::AlignCenter, "Время (мин)");
    pPainter->restore();

    // Подпись "Номер рабочего" под осью X
    pPainter->drawText(marginHorizontal, iScreenHeight - labelOffset, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Номер рабочего");

//     Подписи и бары по оси X (Рабочие)
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
        pPainter->fillRect(xPosition, graphBottom - barHeightTime, halfBarWidth - 1, barHeightTime, Qt::blue);

        // Рисуем красный бар для количества операций рядом
        pPainter->fillRect(xPosition + halfBarWidth + 1, graphBottom - barHeightJob, halfBarWidth - 1, barHeightJob, Qt::red);

        xPosition += barWidth;
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
//        qmworkerJobQount[op.iMachine] += 1;
//    }

//    // Определение максимального значения времени для масштабирования графика
//    int maxTime = *std::max_element(qmworkerTime.begin(), qmworkerTime.end());

//    if (maxTime == 0) {
//        return; // Если нет данных, выход
//    }

//    int numWorkers = qmworkerTime.size();
//    int margin = std::min(iScreenWidth, iScreenHeight) * 0.01;  // например, 5% от меньшей стороны

////    int barWidth = iScreenWidth / (numWorkers + 2); // Расчет ширины бара с учетом отступов
////    int barWidth = std::min((iScreenWidth - 2 * margin) / numWorkers+2, static_cast<int>(iScreenHeight * 0.1));
//    int barWidth = std::min(( (iScreenWidth- 2 * margin) / (numWorkers + 2)), static_cast<int>(iScreenHeight * 0.5, iScreenWidth*0.15));

//    // Масштабные коэффициенты и отступы, зависящие от размера экрана
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

//    // Отрисовка осей
//    pPainter->drawLine(marginHorizontal, graphTop, marginHorizontal, graphBottom); // Y axis
//    pPainter->drawLine(marginHorizontal, graphBottom, iScreenWidth - marginHorizontal, graphBottom); // X axis

//    // Подпись графика сверху
//    pPainter->drawText(marginHorizontal, marginVertical / 2, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Время по рабочим");

//    // Подписи по оси Y (Время в минутах)
//    for (int i = 0; i <= maxTime; i += (maxTime / 10)) {
//        int y = graphBottom - (graphHeight * i / maxTime);
//        pPainter->drawText(0, y - fontSize / 2, marginHorizontal - 10, fontSize*2, Qt::AlignRight, QString::number(i));
//    }

//    // Подпись "Время (мин)" слева от оси Y
////    pPainter->drawText(0, marginVertical, marginHorizontal - 10, labelOffset, Qt::AlignRight, "Время (мин)");

//    // Поворачиваем текст "Время (мин)"
//    int textWidth = pPainter->fontMetrics().horizontalAdvance("Время (мин)");
//    int textHeight = fontSize;
//    pPainter->save();
//    // Вычисляем позицию для текста, так чтобы он был по центру и не съезжал за край экрана
//    int textVerticalPosition = (iScreenHeight / 2)+ textWidth/2;
//    int horizontalMargin = iScreenWidth * 0.01; // Небольшой отступ от края экрана, пропорциональный ширине

//    pPainter->translate(horizontalMargin, textVerticalPosition);
//    pPainter->rotate(-90);
//    pPainter->drawText(0, 0, textWidth, textHeight, Qt::AlignCenter, "Время (мин)");
//    pPainter->restore();


//    // Подпись "Номер рабочего" под осью X
//    pPainter->drawText(marginHorizontal, iScreenHeight - labelOffset, iScreenWidth - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Номер рабочего");

//    // Подписи и бары по оси X (Рабочие)
//    int xPosition = marginHorizontal;
//    for (auto i = qmworkerTime.begin(); i != qmworkerTime.end(); ++i) {
//        int machineId = i.key();
//        int time = i.value();
//        int barHeight = graphHeight * time / maxTime;
//        QString label = QString("Р%1").arg(machineId);
//        pPainter->drawText(xPosition, graphBottom + labelOffset / 2, barWidth, labelOffset, Qt::AlignCenter, label);
//        pPainter->fillRect(xPosition, graphBottom - barHeight, barWidth - 5, barHeight, Qt::blue);
//        xPosition += barWidth;
//    }
//}

