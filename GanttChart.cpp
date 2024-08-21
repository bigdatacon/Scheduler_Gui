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

    // Размеры экрана
    int iScreenWidth = width();
    int iScreenHeight = height();
    int offset_koef = width()*0.025;

    // Масштабирование для оси X
    int iLabelOffsetX = iScreenWidth * 0.1;  // 10% от ширины окна на подписи
    int iScaleFactorX = (iScreenWidth - iLabelOffsetX - offset_koef) / maxFinish;  // Масштабирование по X

    // Динамически рассчитываем высоты и отступы
    int iJobHeight = iScreenHeight * 0.025;  // 5% от высоты окна для каждого job
    int iMachineHeight = iScreenHeight * 0.025;  // 5% от высоты окна для каждой машины

    // Увеличиваем отступы по Y для большего расстояния между графиками
    int iOffsetYJs = iScreenHeight * 0.1;  // Верхний отступ - 10% от высоты экрана
    int iOffsetYMs = iScreenHeight * 0.6;  // Нижний отступ - 60% от высоты экрана

    // Перерисовываем изображение с новыми размерами окна
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);

    QPainter oPainter(&m_oChartImage);
    oPainter.setPen(QPen(Qt::black, 2));

    // Отрисовка осей и сетки с учетом maxFinish
    DrawAxesAndLabels(oPainter, iOffsetYJs, iOffsetYMs, iScaleFactorX, iLabelOffsetX, maxFinish);

    // Отрисовка заголовков сверху
    QFont titleFont = oPainter.font();
    titleFont.setPointSize(iScreenHeight * 0.013);  // Шрифт заголовка в зависимости от высоты окна
    oPainter.setFont(titleFont);
    oPainter.drawText(iLabelOffsetX + (iScreenWidth / 2) - offset_koef, iOffsetYJs - iJobHeight, "Machines");
    oPainter.drawText(iLabelOffsetX + (iScreenWidth / 2) - offset_koef, iOffsetYMs - iJobHeight, "Jobs");

    // Отрисовка js_operations (график машин)
    for (const auto &sOp : m_vJsOperations) {
        int iBarStartX = iLabelOffsetX + sOp.iStart * iScaleFactorX;
        int iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        int iBarY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight;

        QRect oMachineRect(iBarStartX, iBarY, iBarWidth, iMachineHeight * 0.6);  // Высота бара пропорциональна высоте машины
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

        QRect oJobRect(iBarStartX, iBarY, iBarWidth, iJobHeight * 0.6);  // Высота бара пропорциональна высоте работы
        oPainter.fillRect(oJobRect, m_umapMachineColors[sOp.iMachine]);
        oPainter.setPen(QPen(Qt::black, 1));
        oPainter.drawRect(oJobRect);
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
