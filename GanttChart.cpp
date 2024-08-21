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
    resize(1824, 1568); // Устанавливаем начальный размер, можно будет развернуть на полный экран
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
    int maxFinish = 0;
    for (const auto &op : m_vJsOperations) {
        if (op.iFinish > maxFinish) {
            maxFinish = op.iFinish;
        }
    }
    for (const auto &op : m_vMsOperations) {
        if (op.iFinish > maxFinish) {
            maxFinish = op.iFinish;
        }
    }
    // Если maxFinish равен 0, устанавливаем его в 1, чтобы избежать деления на ноль
    if (maxFinish == 0) {
        maxFinish = 1;
    }
    // Определяем отступы и масштабирование
    int iLabelOffsetX = 150;  // Начальное значение для iLabelOffsetX
//    int iScaleFactorX = (width() - iLabelOffsetX - 50) / maxFinish;  // Масштабирование по X
    int iScaleFactorX = (width() - iLabelOffsetX - 50) / maxFinish;


    // Перерисовываем изображение с новыми размерами окна
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    QPainter oPainter(&m_oChartImage);
    oPainter.setPen(QPen(Qt::black, 2));

    int iJobHeight = 40;
    int iMachineHeight = 40;

    // Увеличиваем отступы по Y для большего расстояния между графиками
    int iOffsetYJs = 100;
    int iOffsetYMs = 500;  // Увеличиваем отступ для нижнего графика

    // Отрисовка осей и сетки с учетом maxFinish
    DrawAxesAndLabels(oPainter, iOffsetYJs, iOffsetYMs, iScaleFactorX, iLabelOffsetX, maxFinish);

    // Отрисовка заголовков сверху
    QFont titleFont = oPainter.font();
    titleFont.setPointSize(16);
    oPainter.setFont(titleFont);
    oPainter.drawText(iLabelOffsetX + (width() / 2) - 50, iOffsetYJs - 40, "Machines");
    oPainter.drawText(iLabelOffsetX + (width() / 2) - 50, iOffsetYMs - 40, "Jobs");

    // Отрисовка js_operations (график машин)
    for (const auto &sOp : m_vJsOperations) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight;

        QRect oMachineRect(iBarStartX, iBarY, iBarWidth, iMachineHeight * 0.4);
        oPainter.fillRect(oMachineRect, m_umapJobColors[sOp.iJob]);
        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawRect(oMachineRect);
        oPainter.drawText(oMachineRect, Qt::AlignCenter, QString("Job %1").arg(sOp.iJob));
    }

    // Отрисовка ms_operations (график задач)
    for (const auto &sOp : m_vMsOperations) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarY = iOffsetYMs + (sOp.iJob - 1) * iJobHeight;

        QRect oJobRect(iBarStartX, iBarY, iBarWidth, iJobHeight * 0.4);
        oPainter.fillRect(oJobRect, m_umapMachineColors[sOp.iMachine]);
        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawRect(oJobRect);
        oPainter.drawText(oJobRect, Qt::AlignCenter, QString("Machine %1").arg(sOp.iMachine));
    }

    update();
}


//void GanttChart::resizeEvent(QResizeEvent *event) {
//    // Пересоздаем изображение с новыми размерами окна
//    m_oChartImage = QImage(event->size(), QImage::Format_ARGB32);
//    m_oChartImage.fill(Qt::white);

//    // Перерисовываем график с учетом нового размера окна
//    DrawGanttChart();
//}


//void GanttChart::DrawGanttChart() {
//    QPainter oPainter(&m_oChartImage);
//    oPainter.setPen(QPen(Qt::black, 2));  // Сделаем оси толще

//    int iJobHeight = 40;  // Фиксированная высота для каждого job/machine
//    int iMachineHeight = 40;

//    int iOffsetYJs = 50;  // Смещение по Y для верхнего графика (машины)
//    int iOffsetYMs = 400; // Смещение по Y для нижнего графика (задачи)
//    int iLabelOffsetX = 160; // Увеличенный отступ для подписей осей
//    int iScaleFactorX = 5;  // Масштабирование по X

//    // 1. Отрисовка осей и подписей
//    DrawAxesAndLabels(oPainter, iOffsetYJs, iOffsetYMs, iScaleFactorX, iLabelOffsetX);

//    // 2. Отрисовка js_operations (график машин)
//    for (const auto &sOp : m_vJsOperations) {
//        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
//        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
//        int iBarY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight;

//        QRect oMachineRect(iBarStartX, iBarY, iBarWidth, iMachineHeight * 0.4);
//        oPainter.fillRect(oMachineRect, m_umapJobColors[sOp.iJob]);
//        oPainter.setPen(QPen(Qt::black, 1));
//        oPainter.drawRect(oMachineRect);  // Черная рамка вокруг бара
//        oPainter.drawText(oMachineRect, Qt::AlignCenter, QString("Job %1").arg(sOp.iJob));
//    }

//    // 3. Отрисовка ms_operations (график задач)
//    for (const auto &sOp : m_vMsOperations) {
//        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
//        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
//        int iBarY = iOffsetYMs + (sOp.iJob - 1) * iJobHeight;

//        QRect oJobRect(iBarStartX, iBarY, iBarWidth, iJobHeight * 0.4);
//        oPainter.fillRect(oJobRect, m_umapMachineColors[sOp.iMachine]);
//        oPainter.setPen(QPen(Qt::black, 1));
//        oPainter.drawRect(oJobRect);  // Черная рамка вокруг бара
//        oPainter.drawText(oJobRect, Qt::AlignCenter, QString("Machine %1").arg(sOp.iMachine));
//    }

//    update();
//}

void GanttChart::DrawAxesAndLabels(QPainter &oPainter, int iOffsetYJs, int iOffsetYMs, int iScaleFactorX, int iLabelOffsetX, int maxFinish) {
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
    font.setPointSize(12);
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
    verticalFont.setPointSize(14);
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
