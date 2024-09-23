#include "GanttChart.h"
#include "JsonReader.h"
#include <random>
#include <QDebug>
#include <iostream>

void GanttChart::LoadJsonData(const QString &sFilename) {
    JsonReader oReader;
    oReader.ReadOperationsFromFile(sFilename, /*m_vJsOperations, m_vMsOperations*/ m_vJsOperations_cont, m_vMsOperations_cont);
    InitializeColors();
}


void GanttChart::LoadJsonData_2(const QString &sFilename) {
    JsonReader oReader;
    oReader.ReadOperationsFromFile_2(sFilename, /*m_vJsOperations, m_vMsOperations*/ m_vJsOperations_cont, m_vMsOperations_cont);
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


//void GanttChart::InitializeColors() {
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dis(0, 255);

//    // Очистка предыдущих цветов
//    m_umapJobColors.clear();
////    m_umapMachineColors.clear();

//    // Создание уникальных цветов для каждого job
//    for (const auto &sOp : m_vJsOperations_cont) {
//        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
//            m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
//        }
//    }

//}


void GanttChart::DrawGanttChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight) {


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


//    for (const auto &op : m_vJsOperations_cont) {
//        if (op.iMachine > iMaxMachine) {
//            iMaxMachine = op.iMachine;
//        }
//    }

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

    for (const auto &op : m_vMsOperations_cont) {
        if (op.iJob > iMaxJob) {
            iMaxJob = op.iJob;
        }
    }

    float fMachineRowCount = iMaxMachine ;  // Количество строк для машин
    float fJobRowCount = iMaxJob ;          // Количество строк для задач

    // Перерисовываем изображение с новыми размерами окна
    QImage m_oChartImage = QImage(iScreenWidth, iScreenHeight, QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    pPainter->setPen(QPen(Qt::black, 2));

    QFontMetrics oMetrics(pPainter->font());
    int iLabelWidth = oMetrics.horizontalAdvance("Рабочие 10000");
    int iLabelWidthWithPadding = static_cast<int>(iLabelWidth * 1.2);

    int iOffset1 = std::max(static_cast<int>(iScreenWidth * 0.05), 50);
    int iOffset3 = std::max(static_cast<int>(iScreenWidth * 0.015), 10);

    int iMinOffset2 = static_cast<int>(iLabelWidthWithPadding * 0.5);
    int iMaxOffset2 = iLabelWidthWithPadding;
    int iOffset2 = std::min(std::max(static_cast<int>(iScreenWidth * 0.2), iMinOffset2), iMaxOffset2);

    int iLabelBoundX = iOffset1 + iOffset2;
    int iLabelOffsetX = iLabelBoundX + iOffset3;
    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - iOffset1) / iMaxFinish;


    // Определение отступов от высоты экрана
    int offsetToXAxisLabels = iScreenHeight * 0.05;  // Отступ от оси X до подписей значений по оси X
    int offsetToTimeLabel = iScreenHeight * 0.03;    // Отступ от подписей значений по оси X до подписи "Время (мин)"
    int offsetToBottomEdge = iScreenHeight * 0.04;   // Отступ от подписи "Время (мин)" до нижнего края экрана или до подписи "Диаграмма по деталям"

    // Вычисляем отступы в процентном соотношении от высоты экрана
    int iMinTopOffset = static_cast<int>(iScreenHeight * 0.02);  // 2% от высоты экрана
    int iMinBottomOffset = static_cast<int>(iScreenHeight * 0.05);  // 5% от высоты экрана

    // Расчет доступной высоты для графиков, учитывая новый отступ снизу
    int availableHeight = iScreenHeight - iScreenHeight * 0.1 - offsetToBottomEdge*2 - offsetToTimeLabel-offsetToXAxisLabels -iMinBottomOffset;

//    int iMachineHeight = availableHeight * 0.07;
//    int iJobHeight = availableHeight * 0.07;

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


    int iOffsetYJs = std::max(static_cast<int>(iScreenHeight * 0.1), iMinTopOffset);
//    int iOffsetYMs = iMachineHeight*2 + fMachineRowCount * iMachineHeight + std::max(static_cast<int>(iScreenHeight * 0.05), iMinTopOffset)+offsetToXAxisLabels;
    // Используем iMinBottomOffset для расчета позиции нижнего графика
    int iOffsetYMs = iMachineHeight * 2 + fMachineRowCount * iMachineHeight + std::max(static_cast<int>(iScreenHeight * 0.05), iMinTopOffset) + offsetToXAxisLabels + iMinBottomOffset;

    // Расчет высоты, доступной для графиков, учитывая новый отступ снизу
    // Учет отступов для подписей
    int labelPadding = static_cast<int>(iScreenHeight * 0.02);
    int iBottomOffset = static_cast<int>(iScreenHeight * 0.1); // 10% от нижнего края экрана

    // Подписи графиков
//    pPainter->setPen(QPen(Qt::black, 1));



    int midPointMachine = iLabelOffsetX + (iScreenWidth - iLabelOffsetX) / 2 - oMetrics.horizontalAdvance("Диаграмма по рабочим") / 2;
    int midPointJob = iLabelOffsetX + (iScreenWidth - iLabelOffsetX) / 2 - oMetrics.horizontalAdvance("Диаграмма по деталям") / 2;

    int iFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.015));
    QFont oFont = pPainter->font();
    oFont.setPointSize(iFontSize);
    pPainter->setFont(oFont);

    oFont.setBold(true); // Установка жирного шрифта
    pPainter->setPen(QPen(Qt::black, 2)); // Жирный и черный контур текста
    // Отрисовка заголовков графиков
    pPainter->drawText(midPointMachine, iOffsetYJs - iScreenHeight * 0.01, "Диаграмма по рабочим");
    pPainter->drawText(midPointJob, iOffsetYMs - iScreenHeight * 0.01, "Диаграмма по деталям");


    QPen oGridPen(QColor(0, 0, 0, 50));
    pPainter->setPen(oGridPen);

    qDebug() << "iOffsetYJs:" << iOffsetYJs << "iOffsetYMs:" << iOffsetYMs;
    qDebug() << "fMachineRowCount:" << fMachineRowCount << "fJobRowCount:" << fJobRowCount;



//    // Центрирование подписи "Время (мин)" между графиками
    int timeLabelX = iLabelOffsetX + (iScreenWidth - iLabelOffsetX) / 2 - iLabelWidth / 2;
    int labelOffset = static_cast<int>(iScreenHeight * 0.04); // Отступ для "Время (мин)" пропорционально высоте экрана

    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
    pPainter->drawText(timeLabelX, iOffsetYJs + fMachineRowCount * iMachineHeight + labelOffset, "Время (мин)");
//    pPainter->drawText(timeLabelX, iOffsetYMs + fJobRowCount * iMachineHeight + labelOffset, "Время (мин)");


    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
    int labelPositionY = iOffsetYMs + fJobRowCount * iMachineHeight + labelOffset;
    int availableSpaceForLabel = iScreenHeight - labelPositionY;

    // Проверяем, достаточно ли места для отступа снизу
    if (availableSpaceForLabel < iMinBottomOffset) {
        labelPositionY = iScreenHeight - iMinBottomOffset;  // Поднимаем подпись вверх, чтобы обеспечить отступ снизу
    }

    pPainter->drawText(timeLabelX, labelPositionY, "Время (мин)");


    int timeLabelYBottom =iOffsetYMs + fJobRowCount * iMachineHeight + labelOffset;
    // Убедимся, что подпись не заезжает на график
    if (timeLabelYBottom > iScreenHeight - 10) { // Если подпись слишком низко, поднимаем выше
        timeLabelYBottom = iScreenHeight - 10;
        pPainter->drawText(timeLabelX, timeLabelYBottom, "Время (мин)");
    }

    for (int i = 0; i <= iMaxFinish; i += 10) {
        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iOffsetYJs + fMachineRowCount * iMachineHeight);
        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + fJobRowCount * iMachineHeight);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + fMachineRowCount * iMachineHeight +iScreenHeight * 0.01, QString::number(i));
        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + fJobRowCount * iMachineHeight + iScreenHeight * 0.01 , QString::number(i));
        pPainter->setPen(oGridPen);
    }

    for (int i = 0; i < fMachineRowCount; ++i) {
        int iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1)), iYPos + 5, QString("Р %1").arg(i + 1));
        pPainter->setPen(oGridPen);
    }

    for (int i = 0; i < fJobRowCount; ++i) {
        int iYPos = iOffsetYMs + (i + 0.5) * iMachineHeight;
        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)), iYPos + 5, QString("Д %1").arg(i + 1));
        pPainter->setPen(oGridPen);
    }

    int distance = iLabelWidth / 2;  // Например, 1/3 от ширины текста

    QFont oVerticalFont;
//    oVerticalFont.setBold(true); // Установка жирного шрифта
    oVerticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));

    // Перерисовываем шрифт для "Рабочие"
    pPainter->setFont(oVerticalFont);
    pPainter->setPen(QPen(Qt::black, 2)); // Жирный и черный контур текста

    pPainter->save();
    QFontMetrics oMachineMetrics(pPainter->font());
    int iMachineTextHeight = oMachineMetrics.height();
    int iMachineCenterY = (iOffsetYJs + iOffsetYJs + fMachineRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2;
//    pPainter->translate(iOffset1 - 10, iMachineCenterY);
    pPainter->translate(iOffset1 + distance, iMachineCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Рабочие");
    pPainter->restore();

    // Перерисовываем шрифт для "Детали"
    pPainter->setFont(oVerticalFont);

    pPainter->save();
    QFontMetrics oJobMetrics(pPainter->font());
    int iJobTextHeight = oJobMetrics.height();
    int iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iMachineHeight) / 2 + iJobTextHeight / 2;
//    pPainter->translate(iOffset1 - 10, iJobCenterY);
    pPainter->translate(iOffset1 + distance, iJobCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Детали");
    pPainter->restore();

    // Отрисовка баров для операций на графике машин (m_vMsOperations_cont)
    for (const auto &sOp : m_vMsOperations_cont) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight + iMachineHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки машины
        QRect oMachineRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

        // Уменьшаем ширину и отрисовываем бар
        pPainter->fillRect(oMachineRect, m_umapJobColors[sOp.iJob]);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawRect(oMachineRect);

        // Устанавливаем шрифт на основе высоты окна
        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
        QFont dynamicFont = pPainter->font();
        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
        pPainter->setFont(dynamicFont);

        // Формируем текст для подписи
        QString labelText;
        labelText = QString("Д %1").arg(sOp.iJob);  // Полная подпись для машины
        // Отрисовка текста
        pPainter->drawText(oMachineRect, Qt::AlignCenter, labelText);
    }

    // Отрисовка баров для операций на графике задач (m_vJsOperations_cont)
    for (const auto &sOp : m_vJsOperations_cont) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iMachineHeight + iMachineHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки задачи
        QRect oJobRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

        // Уменьшаем ширину и отрисовываем бар
        pPainter->fillRect(oJobRect, m_umapJobColors[sOp.iJob]);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawRect(oJobRect);

        // Устанавливаем шрифт на основе высоты окна
        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
        QFont dynamicFont = pPainter->font();
        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
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

//    // Отрисовка баров для операций на графике машин (m_vJsOperations)
//    for (const auto &sOp : m_vJsOperations_cont) {
//        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
//        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
//        int iBarCenterY = iOffsetYJs + (sOp.vMachinesIndexes[0] - 1) * iMachineHeight + iMachineHeight / 2;

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

//        // Проверка на разницу времени и выбор формата подписи
//        QString labelText;
//        labelText = QString("Д %1").arg(sOp.iJob);  // Полная подпись
//        // Отрисовка текста
//        pPainter->drawText(oMachineRect, Qt::AlignCenter, labelText);
//    }

//    // Отрисовка баров для операций на графике задач (m_vMsOperations)
//    for (const auto &sOp : m_vMsOperations_cont) {
//        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
//        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
//        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iMachineHeight + iMachineHeight / 2;

//        // Уменьшаем высоту бара до 50% от высоты строки задачи
//        QRect oJobRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

//        // Уменьшаем ширину и отрисовываем бар
////        pPainter->fillRect(oJobRect, m_umapMachineColors[sOp.iMachine]);
//        pPainter->fillRect(oJobRect, m_umapJobColors[sOp.iJob]);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawRect(oJobRect);

//        // Устанавливаем шрифт на основе высоты окна
//        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
//        QFont dynamicFont = pPainter->font();
//        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
//        pPainter->setFont(dynamicFont);

//        // Проверка на разницу времени и выбор формата подписи
//        QString labelText;
//        labelText = QString("Р %1").arg(sOp.iMachine);  // Полная подпись
//        // Отрисовка текста
//        pPainter->drawText(oJobRect, Qt::AlignCenter, labelText);
//    }
}



