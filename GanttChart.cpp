#include "GanttChart.h"
#include "JsonReader.h"
#include <QPainter>
#include <QMouseEvent>
#include <random>

GanttChart::GanttChart(QWidget *pParent)
    : QWidget(pParent), m_iDraggedJob(-1), m_iDraggedMachine(-1), m_bJsMode(true) {
    Initialize();
}

void GanttChart::Initialize() {
    // Убираем фиксированный размер
    resize(1424, 968); // Устанавливаем начальный размер, можно будет развернуть на полный экран
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);
}

void GanttChart::LoadJsonData(const QString &sFilename) {
    // Загружаем данные из JSON
    JsonReader oReader;
    oReader.ReadOperationsFromFile(sFilename, m_vJsOperations, m_vMsOperations);

    // Инициализируем цвета для jobs и machines
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // Уникальные цвета для js_operations
    for (const auto &sOp : m_vJsOperations) {
        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
            m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
        }
        if (m_umapMachineColors.find(sOp.iMachine) == m_umapMachineColors.end()) {
            m_umapMachineColors[sOp.iMachine] = QColor(dis(gen), dis(gen), dis(gen));
        }
    }

    // Уникальные цвета для ms_operations
    for (const auto &sOp : m_vMsOperations) {
        if (m_umapJobColors.find(sOp.iJob) == m_umapJobColors.end()) {
            m_umapJobColors[sOp.iJob] = QColor(dis(gen), dis(gen), dis(gen));
        }
        if (m_umapMachineColors.find(sOp.iMachine) == m_umapMachineColors.end()) {
            m_umapMachineColors[sOp.iMachine] = QColor(dis(gen), dis(gen), dis(gen));
        }
    }

    DrawGanttChart();
}

void GanttChart::DrawGanttChart() {
    // Рассчитываем максимальное значение Finish для ограничения оси X
    int iMaxFinish = 0;
    for (const auto &op : m_vJsOperations) {
        if (op.iFinish > iMaxFinish) {
            iMaxFinish = op.iFinish;
        }
    }
    for (const auto &op : m_vMsOperations) {
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
    for (const auto &op : m_vJsOperations) {
        if (op.iMachine > iMaxMachine) {
            iMaxMachine = op.iMachine;
        }
    }
    for (const auto &op : m_vMsOperations) {
        if (op.iJob > iMaxJob) {
            iMaxJob = op.iJob;
        }
    }

    float fMachineRowCount = iMaxMachine + 2.0f;  // Количество строк для машин
    float fJobRowCount = iMaxJob + 2.0f;          // Количество строк для задач

    int iScreenWidth = width();
    int iScreenHeight = height();

    // Перерисовываем изображение с новыми размерами окна
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    QPainter oPainter(&m_oChartImage);
    oPainter.setPen(QPen(Qt::black, 2));

    QFontMetrics oMetrics(oPainter.font());
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
    QFont oFont = oPainter.font();
    oFont.setPointSize(iFontSize);
    oPainter.setFont(oFont);

    QPen oGridPen(QColor(0, 0, 0, 50));
    oPainter.setPen(oGridPen);

    for (int i = 0; i <= iMaxFinish; i += 10) {
        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iOffsetYJs + fMachineRowCount * iMachineHeight);
        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + fJobRowCount * iJobHeight);

        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + fMachineRowCount * iMachineHeight + 20, QString::number(i));
        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + fJobRowCount * iJobHeight + 20, QString::number(i));
        oPainter.setPen(oGridPen);
    }

    for (int i = 0; i < fMachineRowCount; ++i) {
        int iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
        oPainter.drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawText(iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Machine %1").arg(i + 1)), iYPos + 5, QString("Machine %1").arg(i + 1));
        oPainter.setPen(oGridPen);
    }

    for (int i = 0; i < fJobRowCount; ++i) {
        int iYPos = iOffsetYMs + (i + 0.5) * iJobHeight;
        oPainter.drawLine(iLabelOffsetX, iYPos, iScreenWidth - iOffset1, iYPos);
        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawText(iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Job %1").arg(i + 1)), iYPos + 5, QString("Job %1").arg(i + 1));
        oPainter.setPen(oGridPen);
    }

    QFont oVerticalFont = oPainter.font();
    oVerticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));
    oPainter.setFont(oVerticalFont);

    oPainter.save();
    QFontMetrics oMachineMetrics(oPainter.font());
    int iMachineTextHeight = oMachineMetrics.height();
    int iMachineCenterY = (iOffsetYJs + iOffsetYJs + fMachineRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2;
    oPainter.translate(iOffset1 - 10, iMachineCenterY);
    oPainter.rotate(-90);
    oPainter.drawText(0, 0, "Machines");
    oPainter.restore();

    oPainter.save();
    QFontMetrics oJobMetrics(oPainter.font());
    int iJobTextHeight = oJobMetrics.height();
    int iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iJobHeight) / 2 + iJobTextHeight / 2;
    oPainter.translate(iOffset1 - 10, iJobCenterY);
    oPainter.rotate(-90);
    oPainter.drawText(0, 0, "Jobs");
    oPainter.restore();

    // Отрисовка баров для операций на графике машин (m_vJsOperations)
    for (const auto &sOp : m_vJsOperations) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight + iMachineHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки машины
        QRect oMachineRect(iBarStartX, iBarCenterY - iMachineHeight * 0.25, iBarWidth, iMachineHeight * 0.5);

        // Уменьшаем ширину и отрисовываем бар
        oPainter.fillRect(oMachineRect, m_umapJobColors[sOp.iJob]);
        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawRect(oMachineRect);


        // Устанавливаем шрифт на основе высоты окна
        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
        QFont dynamicFont = oPainter.font();
        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
        oPainter.setFont(dynamicFont);



        // Отрисовка текста
        oPainter.drawText(oMachineRect, Qt::AlignCenter, QString("Job %1").arg(sOp.iJob));
    }

    // Отрисовка баров для операций на графике задач (m_vMsOperations)
    for (const auto &sOp : m_vMsOperations) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iJobHeight + iJobHeight / 2;

        // Уменьшаем высоту бара до 50% от высоты строки задачи
        QRect oJobRect(iBarStartX, iBarCenterY - iJobHeight * 0.25, iBarWidth, iJobHeight * 0.5);

        // Уменьшаем ширину и отрисовываем бар
        oPainter.fillRect(oJobRect, m_umapMachineColors[sOp.iMachine]);
        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawRect(oJobRect);

        // Устанавливаем шрифт на основе высоты окна
        int dynamicFontSize = std::max(8, static_cast<int>(iScreenHeight * 0.01));  // Размер шрифта пропорционален высоте окна
        QFont dynamicFont = oPainter.font();
        dynamicFont.setPointSize(dynamicFontSize);  // Привязываем размер шрифта к размеру окна
        oPainter.setFont(dynamicFont);


        // Отрисовка текста
        oPainter.drawText(oJobRect, Qt::AlignCenter, QString("Machine %1").arg(sOp.iMachine));
    }


    update();
}

void GanttChart::resizeEvent(QResizeEvent *event) {
    // Пересоздаем изображение с новыми размерами окна
    m_oChartImage = QImage(event->size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    // Перерисовываем график с учетом нового размера окна
    DrawGanttChart();
}


void GanttChart::paintEvent(QPaintEvent * /* event */) {
    QPainter oPainter(this);
    oPainter.drawImage(0, 0, m_oChartImage);
}

void GanttChart::mousePressEvent(QMouseEvent *event) {
    m_bJsMode = (event->pos().y() < 300);  // Верхний график

    const auto &operations = m_bJsMode ? m_vJsOperations : m_vMsOperations;
    for (const auto &sOp : operations) {
        QRect oRect(sOp.iStart, sOp.iJob * 40, sOp.iFinish - sOp.iStart, 35); // Check within Job bars
        if (oRect.contains(event->pos())) {
            m_iDraggedJob = sOp.iJob;
            m_iDraggedMachine = sOp.iMachine;
            m_qDragStartPosition = event->pos();
            return;
        }
    }
}

void GanttChart::mouseMoveEvent(QMouseEvent *event) {
    if (m_iDraggedJob != -1 && (event->buttons() & Qt::LeftButton)) {
        int iDeltaX = event->pos().x() - m_qDragStartPosition.x();

        int iNewJob = (m_bJsMode ? event->pos().y() / 40 : (event->pos().y() - 300) / 40);  // Новая позиция job

        UpdateBar(m_iDraggedJob, m_iDraggedMachine, iDeltaX, iNewJob);
        m_qDragStartPosition = event->pos();  // Обновляем стартовую позицию
    }
}

void GanttChart::UpdateBar(int iJob, int iMachine, int iNewStart, int iNewJob) {
    auto &operations = m_bJsMode ? m_vJsOperations : m_vMsOperations;

    for (auto &sOp : operations) {
        if (sOp.iJob == iJob && sOp.iMachine == iMachine) {
            sOp.iStart += iNewStart;
            sOp.iFinish += iNewStart;
            sOp.iJob = iNewJob;

            QPainter oPainter(&m_oChartImage);
            int iYOffset = m_bJsMode ? 0 : 300;
            QRect oMachineRect(sOp.iStart, iYOffset + sOp.iJob * 40, sOp.iFinish - sOp.iStart, 35);
            oPainter.fillRect(oMachineRect, m_umapMachineColors[sOp.iMachine]);
            oPainter.drawText(oMachineRect, Qt::AlignCenter, QString("Machine %1").arg(sOp.iMachine));

            update();
            break;
        }
    }
}
