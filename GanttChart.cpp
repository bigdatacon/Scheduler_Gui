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
    int maxFinish = 160;

    // Размеры экрана
    int iScreenWidth = width();
    int iScreenHeight = height();

    // Перерисовываем изображение с новыми размерами окна
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    QPainter oPainter(&m_oChartImage);
    oPainter.setPen(QPen(Qt::black, 2));

    QFontMetrics metrics(oPainter.font());

    // Рассчитываем длину строки "Machine 10000"
    int labelWidth = metrics.horizontalAdvance("Machine 10000");

    // Увеличиваем длину на 20%
    int labelWidthWithPadding = static_cast<int>(labelWidth * 1.2);

    // Пропорциональные отступы от краев окна, приводим оба значения к int
    int offset1 = std::max(static_cast<int>(iScreenWidth * 0.05), 50);  // Отступ 1: от левого края до вертикальных подписей
    int offset3 = std::max(static_cast<int>(iScreenWidth * 0.015), 10);  // Минимальный отступ между подписями по оси Y и самой осью Y

    // Рассчитываем offset2 как увеличенную на 20% длину строки, с учетом масштаба окна
    int minOffset2 = static_cast<int>(labelWidthWithPadding * 0.5);  // Минимальное значение offset2 при маленьком окне
    int maxOffset2 = labelWidthWithPadding;  // Максимальное значение offset2 при большом окне
    int offset2 = std::min(std::max(static_cast<int>(iScreenWidth * 0.2), minOffset2), maxOffset2);

    // Общий отступ для начала оси X (зависит от отступов и ширины подписи)
    int iLabelBoundX = offset1 + offset2;

    // Масштабирование для оси X
    int iLabelOffsetX = iLabelBoundX + offset3;  // Отступ осей X
    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - offset1) / maxFinish;  // Масштабирование по X

    // Динамически рассчитываем высоты и отступы
    int iMachineHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);  // Ограничение на высоту строки (7% от высоты экрана или 40px)
    int iJobHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);      // Ограничение на высоту строки

    // Увеличиваем отступы по Y для большего расстояния между графиками
    int iOffsetYJs = static_cast<int>(iScreenHeight * 0.1);  // Верхний отступ - 10% от высоты экрана
    int iOffsetYMs = iOffsetYJs + 7 * iMachineHeight + static_cast<int>(iScreenHeight * 0.05);  // Нижний график ниже верхнего на 5% высоты экрана

    // Устанавливаем шрифт
    int fontSize = std::max(8, static_cast<int>(iScreenHeight * 0.015));
    QFont font = oPainter.font();
    font.setPointSize(fontSize);
    oPainter.setFont(font);

    // Отрисовка сетки и подписей на оси X
    QPen gridPen(QColor(0, 0, 0, 50));  // Прозрачный черный цвет для сетки
    oPainter.setPen(gridPen);

    for (int i = 0; i <= maxFinish; i += 10) {
        // Отрисовка вертикальных линий сетки
        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iOffsetYJs + 7 * iMachineHeight);
        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + 4 * iJobHeight);

        // Подписи по оси X
        oPainter.setPen(QPen(Qt::black, 1));  // Черный цвет для подписей
        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + 7 * iMachineHeight + 20, QString::number(i));
        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + 4 * iJobHeight + 20, QString::number(i));
        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
    }

    // Отрисовка горизонтальных линий сетки для машин
    for (int i = 0; i < 7; ++i) {
        int yPos = iOffsetYJs + (i + 0.5) * iMachineHeight;  // Смещение линии сетки
        oPainter.drawLine(iLabelOffsetX, yPos, iScreenWidth - offset1, yPos);
        oPainter.setPen(QPen(Qt::black, 1));

        // Подписи для машин, располагаем с отступом от оси Y
        oPainter.drawText(iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Machine %1").arg(i + 1)), yPos + 5, QString("Machine %1").arg(i + 1));

        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
    }

    // Отрисовка горизонтальных линий сетки для работ
    for (int i = 0; i < 4; ++i) {
        int yPos = iOffsetYMs + (i + 0.5) * iJobHeight;  // Смещение линии сетки
        oPainter.drawLine(iLabelOffsetX, yPos, iScreenWidth - offset1, yPos);
        oPainter.setPen(QPen(Qt::black, 1));

        // Подписи для работ, располагаем с отступом от оси Y
        oPainter.drawText(iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Job %1").arg(i + 1)), yPos + 5, QString("Job %1").arg(i + 1));
        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
    }

    // Устанавливаем шрифт для вертикальных подписей
    QFont verticalFont = oPainter.font();
    verticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));  // Масштабируем размер текста вертикальных подписей
    oPainter.setFont(verticalFont);

    // Подпись для "Machines" слева от графика, с отступом от левого края экрана
    oPainter.save();
    oPainter.translate(offset1 - 10, iOffsetYJs + (7 * iMachineHeight) / 2);  // Центрируем по высоте графика с дополнительным отступом
    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
    oPainter.drawText(0, 0, "Machines");
    oPainter.restore();

    // Подпись для "Jobs" слева от графика, с отступом от левого края экрана
    oPainter.save();
    oPainter.translate(offset1 - 10, iOffsetYMs + (4 * iJobHeight) / 2);  // Центрируем по высоте графика с дополнительным отступом
    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
    oPainter.drawText(0, 0, "Jobs");
    oPainter.restore();

    update();
}



//void GanttChart::DrawGanttChart() {
//    // Рассчитываем максимальное значение Finish для ограничения оси X
//    int maxFinish = 160;

//    // Размеры экрана
//    int iScreenWidth = width();
//    int iScreenHeight = height();



//    // Перерисовываем изображение с новыми размерами окна
//    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
//    m_oChartImage.fill(Qt::white);

//    QPainter oPainter(&m_oChartImage);
//    oPainter.setPen(QPen(Qt::black, 2));

//    QFontMetrics metrics(oPainter.font());

//    // Рассчитываем длину строки "Machine 10000"
//    int labelWidth = metrics.horizontalAdvance("Machine 10000");

//    // Увеличиваем длину на 20%
//    int labelWidthWithPadding = static_cast<int>(labelWidth * 1.2);

//    // Пропорциональные отступы от краев окна, приводим оба значения к int
//    int offset1 = std::max(static_cast<int>(iScreenWidth * 0.05), 50);  // Отступ 1: от левого края до вертикальных подписей

////    int offset2 = std::max(static_cast<int>(iScreenWidth * 0.03), 30);  // Минимальный отступ между вертикальными подписями и подписями по оси Y
//    int offset3 = std::max(static_cast<int>(iScreenWidth * 0.015), 10);  // Уменьшенный отступ между подписями по оси Y и самой осью Y

//    // Рассчитываем offset2 как сумму offset3 и увеличенной на 20% длины строки
//    int offset2 = offset3 + labelWidthWithPadding;

//    // Общий отступ для начала оси X
//    int iLabelBoundX = offset1 + offset2 + offset3;

//    // Масштабирование для оси X
//    int iLabelOffsetX = iLabelBoundX + offset3;  // Отступ осей X
//    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - offset1) / maxFinish;  // Масштабирование по X

//    // Динамически рассчитываем высоты и отступы
//    int iMachineHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);  // Ограничение на высоту строки (7% от высоты экрана или 40px)
//    int iJobHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);      // Ограничение на высоту строки

//    // Увеличиваем отступы по Y для большего расстояния между графиками
//    int iOffsetYJs = static_cast<int>(iScreenHeight * 0.1);  // Верхний отступ - 10% от высоты экрана
//    int iOffsetYMs = iOffsetYJs + 7 * iMachineHeight + static_cast<int>(iScreenHeight * 0.05);  // Нижний график ниже верхнего на 5% высоты экрана


//    // Устанавливаем шрифт
//    int fontSize = std::max(8, static_cast<int>(iScreenHeight * 0.015));
//    QFont font = oPainter.font();
//    font.setPointSize(fontSize);
//    oPainter.setFont(font);

//    // Отрисовка сетки и подписей на оси X
//    QPen gridPen(QColor(0, 0, 0, 50));  // Прозрачный черный цвет для сетки
//    oPainter.setPen(gridPen);

//    for (int i = 0; i <= maxFinish; i += 10) {
//        // Отрисовка вертикальных линий сетки
//        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iOffsetYJs + 7 * iMachineHeight);
//        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + 4 * iJobHeight);

//        // Подписи по оси X
//        oPainter.setPen(QPen(Qt::black, 1));  // Черный цвет для подписей
//        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + 7 * iMachineHeight + 20, QString::number(i));
//        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + 4 * iJobHeight + 20, QString::number(i));
//        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
//    }

//    // Отрисовка горизонтальных линий сетки для машин
//    for (int i = 0; i < 7; ++i) {
//        int yPos = iOffsetYJs + (i + 0.5) * iMachineHeight;  // Смещение линии сетки
//        oPainter.drawLine(iLabelOffsetX, yPos, iScreenWidth - offset1, yPos);
//        oPainter.setPen(QPen(Qt::black, 1));

//        // Подписи для машин выравниваются по линиям сетки и не заезжают на график
//        int offsetXLabels = iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Machine %1").arg(i + 1)) - offset2;
//        oPainter.drawText(offsetXLabels, yPos + 5, QString("Machine %1").arg(i + 1));

//        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
//    }

//    // Отрисовка горизонтальных линий сетки для работ
//    for (int i = 0; i < 4; ++i) {
//        int yPos = iOffsetYMs + (i + 0.5) * iJobHeight;  // Смещение линии сетки
//        oPainter.drawLine(iLabelOffsetX, yPos, iScreenWidth - offset1, yPos);
//        oPainter.setPen(QPen(Qt::black, 1));

//        // Подписи для работ выравниваются по линиям сетки и не заезжают на график
//        int offsetXLabels = iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Job %1").arg(i + 1)) - offset2;
//        oPainter.drawText(offsetXLabels, yPos + 5, QString("Job %1").arg(i + 1));
//        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
//    }

//    // Устанавливаем шрифт для вертикальных подписей
//    QFont verticalFont = oPainter.font();
//    verticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));  // Масштабируем размер текста вертикальных подписей
//    oPainter.setFont(verticalFont);

//    // Подпись для "Machines" слева от графика, с отступом от левого края экрана
//    oPainter.save();
//    oPainter.translate(offset1 - 10, iOffsetYJs + (7 * iMachineHeight) / 2);  // Центрируем по высоте графика с дополнительным отступом
//    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
//    oPainter.drawText(0, 0, "Machines");
//    oPainter.restore();

//    // Подпись для "Jobs" слева от графика, с отступом от левого края экрана
//    oPainter.save();
//    oPainter.translate(offset1 - 10, iOffsetYMs + (4 * iJobHeight) / 2);  // Центрируем по высоте графика с дополнительным отступом
//    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
//    oPainter.drawText(0, 0, "Jobs");
//    oPainter.restore();

//    update();
//}


//void GanttChart::DrawGanttChart() {
//    // Рассчитываем максимальное значение Finish для ограничения оси X
//    int maxFinish = 160;

//    // Размеры экрана
//    int iScreenWidth = width();
//    int iScreenHeight = height();

//    // Пропорциональные отступы от краев окна, приводим оба значения к int
//    int offset1 = std::max(static_cast<int>(iScreenWidth * 0.05), 50);  // Отступ 1: от левого края до вертикальных подписей
//    int offset2 = std::max(static_cast<int>(iScreenWidth * 0.03), 40);  // Отступ 2: между вертикальными подписями и подписями по оси Y
//    int offset3 = std::max(static_cast<int>(iScreenWidth * 0.03), 30);  // Отступ 3: между подписями по оси Y и самой осью Y

//    // Общий отступ для начала оси X
//    int iLabelBoundX = offset1 + offset2 + offset3;

//    // Масштабирование для оси X
//    int iLabelOffsetX = iLabelBoundX + offset3;  // Отступ осей X
//    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - offset1) / maxFinish;  // Масштабирование по X

//    // Динамически рассчитываем высоты и отступы
//    int iMachineHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);  // Ограничение на высоту строки (7% от высоты экрана или 40px)
//    int iJobHeight = std::min(static_cast<int>(iScreenHeight * 0.07), 40);      // Ограничение на высоту строки

//    // Увеличиваем отступы по Y для большего расстояния между графиками
//    int iOffsetYJs = static_cast<int>(iScreenHeight * 0.1);  // Верхний отступ - 10% от высоты экрана
//    int iOffsetYMs = iOffsetYJs + 7 * iMachineHeight + static_cast<int>(iScreenHeight * 0.05);  // Нижний график ниже верхнего на 5% высоты экрана

//    // Перерисовываем изображение с новыми размерами окна
//    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
//    m_oChartImage.fill(Qt::white);

//    QPainter oPainter(&m_oChartImage);
//    oPainter.setPen(QPen(Qt::black, 2));

//    // Устанавливаем шрифт
//    int fontSize = std::max(8, static_cast<int>(iScreenHeight * 0.015));
//    QFont font = oPainter.font();
//    font.setPointSize(fontSize);
//    oPainter.setFont(font);

//    // Отрисовка сетки и подписей на оси X
//    QPen gridPen(QColor(0, 0, 0, 50));  // Прозрачный черный цвет для сетки
//    oPainter.setPen(gridPen);

//    for (int i = 0; i <= maxFinish; i += 10) {
//        // Отрисовка вертикальных линий сетки
//        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYJs, iLabelOffsetX + i * iScaleFactorX, iOffsetYJs + 7 * iMachineHeight);
//        oPainter.drawLine(iLabelOffsetX + i * iScaleFactorX, iOffsetYMs, iLabelOffsetX + i * iScaleFactorX, iOffsetYMs + 4 * iJobHeight);

//        // Подписи по оси X
//        oPainter.setPen(QPen(Qt::black, 1));  // Черный цвет для подписей
//        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYJs + 7 * iMachineHeight + 20, QString::number(i));
//        oPainter.drawText(iLabelOffsetX + i * iScaleFactorX - 10, iOffsetYMs + 4 * iJobHeight + 20, QString::number(i));
//        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
//    }

//    // Отрисовка горизонтальных линий сетки для машин
//    for (int i = 0; i < 7; ++i) {
//        int yPos = iOffsetYJs + (i + 0.5) * iMachineHeight;  // Смещение линии сетки
//        oPainter.drawLine(iLabelOffsetX, yPos, iScreenWidth - offset1, yPos);
//        oPainter.setPen(QPen(Qt::black, 1));

//        // Подписи для машин выравниваются по линиям сетки и не заезжают на график
//        oPainter.drawText(iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Machine %1").arg(i + 1)) - offset3, yPos + 5, QString("Machine %1").arg(i + 1));
////        oPainter.drawText(iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Machine %1").arg(i + 1)) - offset2, yPos + 5, QString("Machine %1").arg(i + 1));


//        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
//    }

//    // Отрисовка горизонтальных линий сетки для работ
//    for (int i = 0; i < 4; ++i) {
//        int yPos = iOffsetYMs + (i + 0.5) * iJobHeight;  // Смещение линии сетки
//        oPainter.drawLine(iLabelOffsetX, yPos, iScreenWidth - offset1, yPos);
//        oPainter.setPen(QPen(Qt::black, 1));

//        // Подписи для работ выравниваются по линиям сетки и не заезжают на гра
//        oPainter.drawText(iLabelBoundX - oPainter.fontMetrics().horizontalAdvance(QString("Job %1").arg(i + 1)) - offset3, yPos + 5, QString("Job %1").arg(i + 1));
//        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
//    }

//    // Устанавливаем шрифт для вертикальных подписей
//    QFont verticalFont = oPainter.font();
//    verticalFont.setPointSize(std::max(10, static_cast<int>(iScreenHeight * 0.02)));  // Масштабируем размер текста вертикальных подписей
//    oPainter.setFont(verticalFont);

//    // Подпись для "Machines" слева от графика, с отступом от левого края экрана
//    oPainter.save();
//    oPainter.translate(offset1, iOffsetYJs + (7 * iMachineHeight) / 2);  // Центрируем по высоте графика
//    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
//    oPainter.drawText(0, 0, "Machines");
//    oPainter.restore();

//    // Подпись для "Jobs" слева от графика, с отступом от левого края экрана
//    oPainter.save();
//    oPainter.translate(offset1, iOffsetYMs + (4 * iJobHeight) / 2);  // Центрируем по высоте графика
//    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
//    oPainter.drawText(0, 0, "Jobs");
//    oPainter.restore();

//    update();
//}


void GanttChart::DrawAxesAndLabels(QPainter &oPainter, int iOffsetYJs, int iOffsetYMs, int iScaleFactorX, int iLabelOffsetX, int maxFinish) {

    // Определение максимального значения machine для верхнего графика и job для нижнего графика
    int maxMachine = 0;
    int maxJob = 0;

    // Проходим по m_vJsOperations, чтобы найти максимальное значение iMachine
    for (const auto &op : m_vJsOperations) {
        if (op.iMachine > maxMachine) {
            maxMachine = op.iMachine;
        }
    }

    // Проходим по m_vMsOperations, чтобы найти максимальное значение iJob
    for (const auto &op : m_vMsOperations) {
        if (op.iJob > maxJob) {
            maxJob = op.iJob;
        }
    }

    // Устанавливаем количество строк для машин и задач, добавляя 2 к максимальному значению
    float machineRowCount = maxMachine + 2.0;  // Количество строк для машин
    float jobRowCount = maxJob + 2.0;          // Количество строк для задач

    // Размеры окна
    int iScreenWidth = width();
    int iScreenHeight = height();

    // Коэффициенты для размеров и отступов
    float labelTextProportion = 0.025;  // Пропорция для размера текста (2.5% от высоты окна)
    float labelOffsetProportion = 0.1;  // Пропорция для отступа слева для подписей
    float lineOffsetProportion = 0.05;  // Пропорция для отступа от краев графика (5% от ширины)
    float rowHeightProportion = 0.05;   // Пропорция для высоты строк (5% от высоты экрана)
    float textHeightOffsetProportion = 0.03; // Пропорция для отступа текста от графиков

    // Рассчитанные размеры и отступы
    int iMachineHeight = iScreenHeight * rowHeightProportion;  // Высота строки машины
    int iJobHeight = iScreenHeight * rowHeightProportion;      // Высота строки задачи
    int textHeightOffset = iScreenHeight * textHeightOffsetProportion;  // Отступ текста под графиком
    int lineOffset = iScreenWidth * lineOffsetProportion;  // Отступ линий графика
    int iLabelTextSize = iScreenHeight * labelTextProportion;  // Размер текста в зависимости от высоты экрана
//    int iDynamicLabelOffsetX = iLabelOffsetX - lineOffset;  // Динамический отступ для подписей



    // Прозрачная сетка внутри графиков
    QPen gridPen(QColor(0, 0, 0, 50));  // Прозрачный черный цвет для сетки
    oPainter.setPen(gridPen);
    // Ось X и Y для js_operations (верхний график)
    for (int i = 0; i <= maxFinish; i += 10) {  // Проходим до maxFinish
        oPainter.setPen(QPen(Qt::black, 1));  // Установим черный цвет для текста
        oPainter.drawText(i * iScaleFactorX + iLabelOffsetX, iOffsetYJs + 6 * 40 + 20, QString::number(i));  // Отметки по X под графиком
        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
        oPainter.drawLine(i * iScaleFactorX + iLabelOffsetX, iOffsetYJs, i * iScaleFactorX + iLabelOffsetX, iOffsetYJs + 6 * 40);  // Вертикальные линии сетки
    }

    // Ось X и Y для ms_operations (нижний график)
    for (int i = 0; i <= maxFinish; i += 10) {  // Проходим до maxFinish
        oPainter.setPen(QPen(Qt::black, 1));  // Установим черный цвет для текста
        oPainter.drawText(i * iScaleFactorX + iLabelOffsetX, iOffsetYMs + 4 * 40 + 20, QString::number(i));  // Отметки по X под графиком
        oPainter.setPen(gridPen);  // Вернем прозрачный цвет для сетки
        oPainter.drawLine(i * iScaleFactorX + iLabelOffsetX, iOffsetYMs, i * iScaleFactorX + iLabelOffsetX, iOffsetYMs + 4 * 40);  // Вертикальные линии сетки
    }

    // Увеличим толщину линий для осей
    QPen axisPen(Qt::black, 2);
    oPainter.setPen(axisPen);

    // Подпись графиков слева
    QFont font = oPainter.font();
//    font.setPointSize(12);
    font.setPointSize(iScreenHeight * 0.012);
    oPainter.setFont(font);

    // Рассчитываем максимальную ширину текста для меток машин и задач
    int maxLabelWidth = 0;
    for (int i = 1; i <= 6; ++i) {
        QString text = QString("Machine %1").arg(i);
        int width = oPainter.fontMetrics().horizontalAdvance(text);
        if (width > maxLabelWidth) {
            maxLabelWidth = width;
        }
    }

    for (int i = 1; i <= 4; ++i) {
        QString text = QString("Job %1").arg(i);
        int width = oPainter.fontMetrics().horizontalAdvance(text);
        if (width > maxLabelWidth) {
            maxLabelWidth = width;
        }
    }

    // Обновляем отступ для меток на основании ширины самой длинной подписи
    int iDynamicLabelOffsetX = iLabelOffsetX - maxLabelWidth - 20;

    // Отрисуем горизонтальные линии для машин с прозрачностью
    for (int i = 1; i <= 6; ++i) {
        oPainter.setPen(gridPen);
        oPainter.drawLine(iDynamicLabelOffsetX + maxLabelWidth + 20, iOffsetYJs + i * 40, width() - 50, iOffsetYJs + i * 40);  // Прозрачная горизонтальная линия
        oPainter.setPen(QPen(Qt::black, 1));  // Вернем черный цвет для текста
        oPainter.drawText(iDynamicLabelOffsetX - 10, iOffsetYJs + i * 40 - 10, QString("Machine %1").arg(i));
    }

    // Отрисуем горизонтальные линии для задач с прозрачностью
    for (int i = 1; i <= 4; ++i) {
        oPainter.setPen(gridPen);
        oPainter.drawLine(iDynamicLabelOffsetX + maxLabelWidth + 20, iOffsetYMs + i * 40, width() - 50, iOffsetYMs + i * 40);  // Прозрачная горизонтальная линия
        oPainter.setPen(QPen(Qt::black, 1));  // Вернем черный цвет для текста
        oPainter.drawText(iDynamicLabelOffsetX - 10, iOffsetYMs + i * 40 - 10, QString("Job %1").arg(i));
    }

    // Рисуем вертикальные названия графиков
    QFont verticalFont = oPainter.font();
//    verticalFont.setPointSize(14);
    verticalFont.setPointSize(iScreenHeight * 0.025);  // Масштабируем размер текста вертикальных подписей

    oPainter.setFont(verticalFont);

    // Рисуем название "Machines" вертикально слева от подписей
    oPainter.save();
    int machinesLabelPosX = iDynamicLabelOffsetX - 50;  // Позиция для текста "Machines"
    oPainter.translate(machinesLabelPosX, (iOffsetYJs + iOffsetYJs + 6 * 40) / 2);  // Центрируем по высоте графика
    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
    oPainter.drawText(0, 0, "Machines");
    oPainter.restore();

    // Рисуем название "Jobs" вертикально слева от подписей
    oPainter.save();
    int jobsLabelPosX = iDynamicLabelOffsetX - 50;  // Позиция для текста "Jobs"
    oPainter.translate(jobsLabelPosX, (iOffsetYMs + iOffsetYMs + 4 * 40) / 2);  // Центрируем по высоте графика
    oPainter.rotate(-90);  // Поворачиваем текст на 90 градусов против часовой стрелки
    oPainter.drawText(0, 0, "Jobs");
    oPainter.restore();
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
