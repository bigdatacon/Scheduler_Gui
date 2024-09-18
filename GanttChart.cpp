#include "GanttChart.h"
#include "JsonReader.h"
#include <random>
#include <QDebug>

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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // Уникальные цвета для js_operations
//    for (const auto &sOp : m_vJsOperations) {
//        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
//            m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
//        }
//        if (m_umapMachineColors.find(sOp.iMachine) == m_umapMachineColors.end()) {
//            m_umapMachineColors[sOp.iMachine] = QColor(dis(gen), dis(gen), dis(gen));
//        }
//    }

//    // Уникальные цвета для ms_operations
//    for (const auto &sOp : m_vMsOperations) {
//        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
//            m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
//        }
//        if (m_umapMachineColors.find(sOp.iMachine) == m_umapMachineColors.end()) {
//            m_umapMachineColors[sOp.iMachine] = QColor(dis(gen), dis(gen), dis(gen));
//        }
//    }

    // Уникальные цвета для js_operations
    for (const auto &sOp : m_vJsOperations_cont) {
        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
            m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
        }
        if (m_umapMachineColors.find(sOp.iMachine) == m_umapMachineColors.end()) {
            m_umapMachineColors[sOp.iMachine] = QColor(dis(gen), dis(gen), dis(gen));
        }
    }

    // Уникальные цвета для ms_operations
    for (const auto &sOp : m_vMsOperations_cont) {
        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
            m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
        }
        if (m_umapMachineColors.find(sOp.iMachine) == m_umapMachineColors.end()) {
            m_umapMachineColors[sOp.iMachine] = QColor(dis(gen), dis(gen), dis(gen));
        }
    }
}



void GanttChart::DrawGanttChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight) {
    // 1) Очищаем предыдущий рисунок, чтобы перерисовать всё по новой
//    pPainter->eraseRect(0, 0, iScreenWidth, iScreenHeight); // Очищаем весь экран перед началом отрисовки


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
        if (op.iMachine > iMaxMachine) {
            iMaxMachine = op.iMachine;
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
    int iLabelWidth = oMetrics.horizontalAdvance("Machine 10000");
    int iLabelWidthWithPadding = static_cast<int>(iLabelWidth * 1.2);

    int iOffset1 = std::max(static_cast<int>(iScreenWidth * 0.05), 50);
    int iOffset3 = std::max(static_cast<int>(iScreenWidth * 0.015), 10);

    int iMinOffset2 = static_cast<int>(iLabelWidthWithPadding * 0.5);
    int iMaxOffset2 = iLabelWidthWithPadding;
    int iOffset2 = std::min(std::max(static_cast<int>(iScreenWidth * 0.2), iMinOffset2), iMaxOffset2);

    int iLabelBoundX = iOffset1 + iOffset2;
    int iLabelOffsetX = iLabelBoundX + iOffset3;
    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - iOffset1) / iMaxFinish;

    int iMachineHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);
    int iJobHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);

    // Добавляем минимальный отступ сверху для графиков
    int iMinTopOffset = 20;
    int iOffsetYJs = std::max(static_cast<int>(iScreenHeight * 0.1), iMinTopOffset);

    // Увеличиваем расстояние между графиками, чтобы подписи не накладывались друг на друга
    int iOffsetYMs = iOffsetYJs + fMachineRowCount * iMachineHeight + std::max(static_cast<int>(iScreenHeight * 0.05), iMinTopOffset);

    int iFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.015));
    QFont oFont = pPainter->font();
    oFont.setPointSize(iFontSize);
    pPainter->setFont(oFont);

    QPen oGridPen(QColor(0, 0, 0, 50));
    pPainter->setPen(oGridPen);

    qDebug() << "iOffsetYJs:" << iOffsetYJs << "iOffsetYMs:" << iOffsetYMs;
    qDebug() << "fMachineRowCount:" << fMachineRowCount << "fJobRowCount:" << fJobRowCount;


    for (int i = 0; i <= iMaxFinish; i += 10) {
        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iOffsetYJs + fMachineRowCount * iMachineHeight);
        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + fJobRowCount * iJobHeight);

        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + fMachineRowCount * iMachineHeight + 20, QString::number(i));
        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + fJobRowCount * iJobHeight + 20, QString::number(i));
        pPainter->setPen(oGridPen);
    }

    for (int i = 0; i < fMachineRowCount; ++i) {
        int iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Machine %1").arg(i + 1)), iYPos + 5, QString("Machine %1").arg(i + 1));
        pPainter->setPen(oGridPen);
    }

    for (int i = 0; i < fJobRowCount; ++i) {
        int iYPos = iOffsetYMs + (i + 0.5) * iJobHeight;
        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Job %1").arg(i + 1)), iYPos + 5, QString("Job %1").arg(i + 1));
        pPainter->setPen(oGridPen);
    }

    QFont oVerticalFont = pPainter->font();
    oVerticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));
    pPainter->setFont(oVerticalFont);

    pPainter->save();
    QFontMetrics oMachineMetrics(pPainter->font());
    int iMachineTextHeight = oMachineMetrics.height();
    int iMachineCenterY = (iOffsetYJs + iOffsetYJs + fMachineRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2;
    pPainter->translate(iOffset1 - 10, iMachineCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Machines");
    pPainter->restore();

    pPainter->save();
    QFontMetrics oJobMetrics(pPainter->font());
    int iJobTextHeight = oJobMetrics.height();
    int iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iJobHeight) / 2 + iJobTextHeight / 2;
    pPainter->translate(iOffset1 - 10, iJobCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Jobs");
    pPainter->restore();

    // Отрисовка баров для операций на графике машин (m_vJsOperations)
    for (const auto &sOp : m_vJsOperations_cont) {
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

        // Проверка на разницу времени и выбор формата подписи
        QString labelText;
        if ((sOp.iFinish - sOp.iStart) < 20) {
            labelText = QString("J%1").arg(sOp.iMachine);  // Короткая подпись
        } else {
            labelText = QString("Job %1").arg(sOp.iJob);  // Полная подпись
        }

        // Отрисовка текста
        pPainter->drawText(oMachineRect, Qt::AlignCenter, labelText);
    }

    // Отрисовка баров для операций на графике задач (m_vMsOperations)
    for (const auto &sOp : m_vMsOperations_cont) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iJobHeight + iJobHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки задачи
        QRect oJobRect(iBarStartX, iBarCenterY - iJobHeight * 0.25, iBarWidth, iJobHeight * 0.5);

        // Уменьшаем ширину и отрисовываем бар
        pPainter->fillRect(oJobRect, m_umapMachineColors[sOp.iMachine]);
        pPainter->setPen(QPen(Qt::black, 1));
        pPainter->drawRect(oJobRect);

        // Устанавливаем шрифт на основе высоты окна
        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
        QFont dynamicFont = pPainter->font();
        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
        pPainter->setFont(dynamicFont);

        // Проверка на разницу времени и выбор формата подписи
        QString labelText;
        if ((sOp.iFinish - sOp.iStart) < 20) {
            labelText = QString("M%1").arg(sOp.iJob);  // Короткая подпись
        } else {
            labelText = QString("Machine %1").arg(sOp.iMachine);  // Полная подпись
        }

        // Отрисовка текста
        pPainter->drawText(oJobRect, Qt::AlignCenter, labelText);
    }


}


//void GanttChart::DrawGanttChart(QPainter *pPainter, int iScreenWidth, int iScreenHeight) {
//    // 1) Очищаем предыдущий рисунок, чтобы перерисовать всё по новой
//    pPainter->eraseRect(0, 0, iScreenWidth, iScreenHeight); // Очищаем весь экран перед началом отрисовки


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
//        if (op.iMachine > iMaxMachine) {
//            iMaxMachine = op.iMachine;
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
//    int iLabelWidth = oMetrics.horizontalAdvance("Machine 10000");
//    int iLabelWidthWithPadding = static_cast<int>(iLabelWidth * 1.2);

//    int iOffset1 = std::max(static_cast<int>(iScreenWidth * 0.05), 50);
//    int iOffset3 = std::max(static_cast<int>(iScreenWidth * 0.015), 10);

//    int iMinOffset2 = static_cast<int>(iLabelWidthWithPadding * 0.5);
//    int iMaxOffset2 = iLabelWidthWithPadding;
//    int iOffset2 = std::min(std::max(static_cast<int>(iScreenWidth * 0.2), iMinOffset2), iMaxOffset2);

//    int iLabelBoundX = iOffset1 + iOffset2;
//    int iLabelOffsetX = iLabelBoundX + iOffset3;
//    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - iOffset1) / iMaxFinish;

//    int iMachineHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);
//    int iJobHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);

//    // Добавляем минимальный отступ сверху для графиков
//    int iMinTopOffset = 20;
//    int iOffsetYJs = std::max(static_cast<int>(iScreenHeight * 0.1), iMinTopOffset);

//    // Увеличиваем расстояние между графиками, чтобы подписи не накладывались друг на друга
//    int iOffsetYMs = iOffsetYJs + fMachineRowCount * iMachineHeight + std::max(static_cast<int>(iScreenHeight * 0.05), iMinTopOffset);

//    int iFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.015));
//    QFont oFont = pPainter->font();
//    oFont.setPointSize(iFontSize);
//    pPainter->setFont(oFont);

//    QPen oGridPen(QColor(0, 0, 0, 50));
//    pPainter->setPen(oGridPen);

//    for (int i = 0; i <= iMaxFinish; i += 10) {
//        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iOffsetYJs + fMachineRowCount * iMachineHeight);
//        pPainter->drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + fJobRowCount * iJobHeight);

//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + fMachineRowCount * iMachineHeight + 20, QString::number(i));
//        pPainter->drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + fJobRowCount * iJobHeight + 20, QString::number(i));
//        pPainter->setPen(oGridPen);
//    }

//    for (int i = 0; i < fMachineRowCount; ++i) {
//        int iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
//        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Machine %1").arg(i + 1)), iYPos + 5, QString("Machine %1").arg(i + 1));
//        pPainter->setPen(oGridPen);
//    }

//    for (int i = 0; i < fJobRowCount; ++i) {
//        int iYPos = iOffsetYMs + (i + 0.5) * iJobHeight;
//        pPainter->drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawText(iLabelBoundX - pPainter->fontMetrics().horizontalAdvance(QString("Job %1").arg(i + 1)), iYPos + 5, QString("Job %1").arg(i + 1));
//        pPainter->setPen(oGridPen);
//    }

//    QFont oVerticalFont = pPainter->font();
//    oVerticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));
//    pPainter->setFont(oVerticalFont);

//    pPainter->save();
//    QFontMetrics oMachineMetrics(pPainter->font());
//    int iMachineTextHeight = oMachineMetrics.height();
//    int iMachineCenterY = (iOffsetYJs + iOffsetYJs + fMachineRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2;
//    pPainter->translate(iOffset1 - 10, iMachineCenterY);
//    pPainter->rotate(-90);
//    pPainter->drawText(0, 0, "Machines");
//    pPainter->restore();

//    pPainter->save();
//    QFontMetrics oJobMetrics(pPainter->font());
//    int iJobTextHeight = oJobMetrics.height();
//    int iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iJobHeight) / 2 + iJobTextHeight / 2;
//    pPainter->translate(iOffset1 - 10, iJobCenterY);
//    pPainter->rotate(-90);
//    pPainter->drawText(0, 0, "Jobs");
//    pPainter->restore();

//    // Отрисовка баров для операций на графике машин (m_vJsOperations)
//    for (const auto &sOp : m_vJsOperations_cont) {
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
////        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
//        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.001));  // Размер шрифта пропорционален высоте окна
//        QFont dynamicFont = pPainter->font();
//        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
//        pPainter->setFont(dynamicFont);



//        // Отрисовка текста
//        pPainter->drawText(oMachineRect, Qt::AlignCenter, QString("Job %1").arg(sOp.iJob));
//    }

//    // Отрисовка баров для операций на графике задач (m_vMsOperations)
//    for (const auto &sOp : m_vMsOperations_cont) {
//        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
//        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
//        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iJobHeight + iJobHeight / 2;

//        // Уменьшаем высоту бара до 50% от высоты строки задачи
//        QRect oJobRect(iBarStartX, iBarCenterY - iJobHeight * 0.25, iBarWidth, iJobHeight * 0.5);

//        // Уменьшаем ширину и отрисовываем бар
//        pPainter->fillRect(oJobRect, m_umapMachineColors[sOp.iMachine]);
//        pPainter->setPen(QPen(Qt::black, 1));
//        pPainter->drawRect(oJobRect);

//        // Устанавливаем шрифт на основе высоты окна
//        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.001));  // Размер шрифта пропорционален высоте окна
//        QFont dynamicFont = pPainter->font();
//        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
//        pPainter->setFont(dynamicFont);


//        // Отрисовка текста
//        pPainter->drawText(oJobRect, Qt::AlignCenter, QString("Machine %1").arg(sOp.iMachine));
//    }
//}
