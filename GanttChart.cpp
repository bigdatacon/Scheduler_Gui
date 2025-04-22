#include "GanttChart.h"
#include <random>
#include <QDebug>
#include <iostream>
#include <QMap>
#include <QTextDocument>
#include <tuple>
#include <set>
#include "GlobalState.h"
#include <algorithm>
#include <QCoreApplication>
#include <QDir>


GanttChart::GanttChart() : m_iZoom(1) {
    m_pSolver = NULL;
}


GanttChart::~GanttChart() 
{
    if ( m_pSolver != NULL )
        delete m_pSolver;
}

void GanttChart::LoadData(const QString &sFilename) 
{


    if ( g_pSolver == NULL ) 
    {
        QString appDirPath = QCoreApplication::applicationDirPath();
        if (!appDirPath.endsWith("/")) {
            appDirPath += "/";
        }
        QString fullPath = sFilename;

        if (!fullPath.startsWith(appDirPath)) {
            fullPath = appDirPath + sFilename;
        }

        QByteArray byteArray = fullPath.toUtf8();
        const char* filenameChar = byteArray.constData();

        if (!std::filesystem::exists(filenameChar)) {
            std::cout << "Ошибка: файл " << filenameChar << " не существует." << std::endl;

        } else {
            std::cout << "Файл " << filenameChar << " существует." << std::endl;
        }

        QFile file(fullPath);
        if (!file.exists()) {
            std::cout << "Ошибка: файл " << sFilename.toStdString() << " не существует." << std::endl;

        } else {
            std::cout << "Файл " << sFilename.toStdString() << " существует в корне." << std::endl;
        }

        g_pSolver = new Solver();
        g_pSolver->LoadProblemFromFileTXT(filenameChar);
        g_iSolverPower = 1;
    }
    else
    {
        if ( g_iSolverPower == 0 )
            g_iSolverPower = 1;
        else
            g_iSolverPower = 2 * g_iSolverPower;
    }

    g_pSolver->SetModes(g_iR1, 0, 0, 0, 1, 0, g_iJ3, 0, 0, 0, 0);
    g_pSolver->SetIterationsCount(30 * g_iSolverPower);
    
    if (g_currentState == DURATION) 
    {
        g_pSolver->SetWeights(1,0,0,0,0,0);
    } 
    else if (g_currentState == COST ) 
    {
        g_pSolver->SetWeights(0,1,0,0,0,0);
    }
    else if (g_currentState == SETUPS) 
    {
        g_pSolver->SetWeights(0,0,1,0,0,0);
    }

    g_bSolverRunning = true;  // Солвер запущен

    g_pSolver->Run();  // Запуск солвера

    g_bSolverRunning = false;  // Солвер завершён

    MSchedule* pMSchedule = g_pSolver->GetMSchedule();
    std::vector<std::vector<MSOperation*>> ms_operations = pMSchedule->GetOperations();

    JSchedule* pJSchedule = g_pSolver->GetJSchedule();
    std::vector<std::vector<JSOperation*>> js_operations = pJSchedule->GetOperations();

    if (ms_operations.empty() ) 
    {
    std::cerr << "Ошибка: ms_operations или js_operations пусты" << std::endl;
    return;
    }

    m_vMsOperations_cont.clear();

    // Сериализация данных для ms_operations
    for (int i = 0; i < ms_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (ms_operations[i].empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями
            m_vMsOperations_cont.push_back(SResourceOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                0,
                0,      // Индекс работы по умолчанию
                0,
//                i + 1   // Машина (индекс +1)
                i
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : ms_operations[i]) {
                m_vMsOperations_cont.push_back(SResourceOperation{
                    op->startTime,
                    op->finishTime,
                    op->setupTime,
//                    op->jobIndex+1,
                    op->jobIndex,
                    op->operationIndex,
//                    i + 1
                    i
                });
            }
        }
    }

    m_vJsOperations_cont.clear();

    // Сериализация данных для j
    for (int i = 0; i < js_operations.size(); ++i) {

        // Проверяем, пустой ли вектор операций

        if (js_operations[i].empty() || js_operations[i][0]->vResourcesIndexes.empty()) {

            // Если вектор пуст, добавляем операцию с нулевыми значениями и пустым вектором индексов машин

            m_vJsOperations_cont.push_back(SJobOperation{

                0,      // Старт по умолчанию

                0,      // Финиш по умолчанию

//                i + 1,  // Индекс работы (индекс +1)
                i,  // Индекс работы (индекс +1)

                0,

                {}      // Пустой вектор машин

            });

        } else {

            // Если операции есть, обрабатываем их
            for (const auto& op : js_operations[i]) 
            {
                // Создаем новый вектор с увеличенными на 1 значениями
                std::vector<int> newResourceIndexes;
                newResourceIndexes.reserve(op->vResourcesIndexes.size());

                for (const auto& index : op->vResourcesIndexes) {
//                    newResourceIndexes.push_back(index + 1);  // Добавляем 1 к каждому значению
                    newResourceIndexes.push_back(index );  // Добавляем 1 к каждому значению
                }

                // Добавляем в контейнер SJobOperation с новым вектором
                m_vJsOperations_cont.push_back(SJobOperation{
                    op->startTime,
                    op->finishTime,
//                    i + 1,
                    i,
                    op->operationIndex,
                    newResourceIndexes  // Используем новый вектор вместо старого
                });
            }

        }

    }

    // Инициализирую структуры
    ScheduleMetrics* pScheduleMetrics = g_pSolver->GetScheduleMetrics();
    m_ScheduleMetrics.iDuration = pScheduleMetrics->iDuration;
    m_ScheduleMetrics.iCost = pScheduleMetrics->iCost; 
    m_ScheduleMetrics.iSumSetupTime = pScheduleMetrics->iSumSetupTime;
    m_ScheduleMetrics.iSumDeviation = 30;
    m_ScheduleMetrics.iDeltaUtilization = 40;
    m_ScheduleMetrics.iFreeResourcesCount = 50;

    //добавляю данные для периодов недоступности
    m_vResources.clear();
    m_vResources =  g_pSolver->GetResources();
    m_vOperation =  g_pSolver->GetOperations();

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

    std::cout << "[DEBUG] machineIndexes from JSOperations:\n";
    for (const auto &op : m_vJsOperations_cont) {
        std::cout << "   Job " << op.iJob << ", Op " << op.iOperation << ": ";
        for (const auto &machineIndex : op.vMachinesIndexes) {
            std::cout << machineIndex << " ";
            if (machineIndex > iMaxMachine) {
                iMaxMachine = machineIndex;
            }
        }
        std::cout << "\n";
    }


    for (const auto &op : m_vMsOperations_cont) {
        if (op.iJob > iMaxJob) {
            iMaxJob = op.iJob;
        }
    }

    // Возвращаем результат в виде кортежа (tuple)
    return std::make_tuple(iMaxFinish, iMaxMachine, iMaxJob);
}


void GanttChart::DrawGanttChart(QPainter *pPainter, int VIRTUAL_SCREEN_WIDTH, int VIRTUAL_SCREEN_HEIGHT) {

    // -- данные по максмальному числу машин и прочего - ///
    auto [iMaxFinish_f, iMaxMachine, iMaxJob] = calculateMaxValues();
    double iMaxFinish = iMaxFinish_f;
//    double fMachineRowCount = iMaxMachine ;  // Количество строк для машин
    double fMachineRowCount = iMaxMachine +1;  // Количество строк для машин
//    double fJobRowCount = iMaxJob ;          // Количество строк для задачdouble fMachineRowCount = iMaxMachine ;  // Количество строк для машин
    double fJobRowCount = iMaxJob +1;          // Количество строк для задачdouble fMachineRowCount = iMaxMachine ;  // Количество строк для машин
    std::cout << "[calculateMaxValues()] MaxFinish: " << iMaxFinish_f
              << ", MaxMachine: " << iMaxMachine
              << ", MaxJob: " << iMaxJob
            << ", fMachineRowCount " << fMachineRowCount
            << ", fJobRowCount: " << fJobRowCount <<

            std::endl;

    int maxMachineIndex = -1;
    int maxJobIndex = -1;
    for (const auto& op : m_vMsOperations_cont) {
        if (op.iMachine > maxMachineIndex) maxMachineIndex = op.iMachine;
    }
    for (const auto& op : m_vJsOperations_cont) {
        if (op.iJob > maxJobIndex) maxJobIndex = op.iJob;
    }
    std::cout << "[REAL] Max iMachine index in data (- mean real data+1): " << maxMachineIndex << ", Max iJob index (- mean real data+1): " << maxJobIndex << std::endl;

     // ------------------------------------------------ ///


    // -------------------------- ОПРЕДЕЛЕНИЕ ШРИФТОВ И ПЁР ----------------------------
    QFont titleFont;
    QFont axisLabelFont;
    QFont dynamicFont;
    QFont verticalFont;
    QFont verticalFontRotate;


    // Вычисляем средний размер экрана для более сбалансированного расчета шрифта
    double iAverageScreenSize = static_cast<int>((VIRTUAL_SCREEN_WIDTH + VIRTUAL_SCREEN_HEIGHT) / 2.5);

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

    // Определение перьев для подписей ресурсов
    QPen resourcePen1(Qt::blue, 1);  // Для станков
    QPen resourcePen2(Qt::darkGreen, 1);  // Для рабочих
    QPen resourcePen3(Qt::magenta, 1);  // Для оснастки

    // Перо для "Время (мин)" (чуть прозрачное и жирное)
    QPen timeLabelPen(QColor(0, 0, 0, 150));  // Черный с прозрачностью 150
    timeLabelPen.setWidth(1);                 // Толщина линии
    timeLabelPen.setStyle(Qt::SolidLine);      // Сплошная линия
    timeLabelPen.setCapStyle(Qt::SquareCap);   // Стиль линий

    //--------------------------------------------------------------------------//

    //-------------------------отступы-----------------------------------------//

    double ioffsetFromVertical =  std::max(static_cast<int>(VIRTUAL_SCREEN_HEIGHT * 0.05), 50);
    double ioffsetFromSide =  std::max(static_cast<int>(VIRTUAL_SCREEN_WIDTH * 0.05), 50);

    double iScaleFactorX = (VIRTUAL_SCREEN_WIDTH - ioffsetFromSide*2) / iMaxFinish;
    //---------------------------------------------------------------------------//

    // Расчет доступной высоты для графиков, учитывая новый отступ снизу
    double availableHeight = VIRTUAL_SCREEN_HEIGHT - ioffsetFromVertical*2 - (ioffsetFromVertical*0.5 * 6 + pPainter->fontMetrics().height());
    float totalRowCount = fMachineRowCount + fJobRowCount;
    float spacingFactor = 1.1;  // С учетом 10% отступа между барами

    // Расчет процентной доли от availableHeight для минимальной и максимальной высоты бара
    float minBarHeightPercentage = 0.01; // 1% от availableHeight
    float maxBarHeightPercentage = 0.10; // 10% от availableHeight

    double minBarHeight = static_cast<int>(availableHeight * minBarHeightPercentage);
    double maxBarHeight = static_cast<int>(availableHeight * maxBarHeightPercentage);

    // Вычисляем начальное значение высоты бара, основываясь на общем количестве строк и доступной высоте
    double iMachineHeight = static_cast<int>(availableHeight / (totalRowCount * spacingFactor));
    // Корректируем высоту баров, чтобы она не выходила за установленные процентные пределы
    iMachineHeight = std::max(minBarHeight, std::min(maxBarHeight, iMachineHeight));

    double iOffsetYJs =  ioffsetFromVertical*2  ;
    double iOffsetYMs = iMachineHeight * 2 + fMachineRowCount * iMachineHeight + ioffsetFromVertical*2 + ioffsetFromVertical*0.7;


    // Подписи графиков
    QFontMetrics oMetrics(pPainter->font());
    double midPointMachine = ioffsetFromSide + (VIRTUAL_SCREEN_WIDTH - ioffsetFromSide) / 2 - oMetrics.horizontalAdvance("Диаграмма по ресурсам") / 2;

    double midPointJob = ioffsetFromSide + (VIRTUAL_SCREEN_WIDTH - ioffsetFromSide) / 2 -  oMetrics.horizontalAdvance("Диаграмма по работам") / 2;

    // Устанавливаем шрифт и перо для заголовков
    pPainter->setFont(titleFont);
    pPainter->setPen(axisPen);

    // Отрисовка заголовков графиков
    pPainter->drawText(midPointMachine, iOffsetYJs - ioffsetFromVertical * 0.8, "Диаграмма по ресурсам");
    pPainter->drawText(midPointJob, iOffsetYMs - VIRTUAL_SCREEN_HEIGHT * 0.01, "Диаграмма по работам");

    // Устанавливаю шрифт  для время мин
    pPainter->setFont(axisLabelFont);
    pPainter->setPen(timeLabelPen);

    // Создаём строку с данными из ScheduleMetrics
    QString scheduleMetricsText;

    if ( g_currentState == DURATION )
    {
        scheduleMetricsText = QString("<b>Длительность: %1</b>, Стоимость: %2, Суммарное время переналадок: %3   (Мощность солвера: %4)")
        .arg(m_ScheduleMetrics.iDuration)
        .arg(m_ScheduleMetrics.iCost)
        .arg(m_ScheduleMetrics.iSumSetupTime)
        .arg(g_iSolverPower);
    }
    else if ( g_currentState == COST )
    {
        scheduleMetricsText = QString("Длительность: %1, <b>Стоимость: %2</b>, Суммарное время переналадок: %3   (Мощность солвера: %4)")
        .arg(m_ScheduleMetrics.iDuration)
        .arg(m_ScheduleMetrics.iCost)
        .arg(m_ScheduleMetrics.iSumSetupTime)
        .arg(g_iSolverPower);
    }
    else if ( g_currentState == SETUPS )
    {
        scheduleMetricsText = QString("Длительность: %1, Стоимость: %2, <b>Суммарное время переналадок: %3</b>   (Мощность солвера: %4)")
        .arg(m_ScheduleMetrics.iDuration)
        .arg(m_ScheduleMetrics.iCost)
        .arg(m_ScheduleMetrics.iSumSetupTime)
        .arg(g_iSolverPower);
    }

    // Используем эту строку для отрисовки текста
    double midPointMachineDuration = ioffsetFromSide + (VIRTUAL_SCREEN_WIDTH - ioffsetFromSide) / 2 - oMetrics.horizontalAdvance(QString("Длительность: Суммарное время переналадок: (Мощность солвера: %3)")) / 2;

    // Указываем QPainter, что текст содержит HTML
    QTextDocument doc;
    doc.setHtml(scheduleMetricsText);
    doc.setDefaultFont(pPainter->font());

    // Определяем координаты и отрисовываем текст
    pPainter->save(); // Сохраняем текущее состояние QPainter
    pPainter->translate(midPointMachineDuration, iOffsetYJs - ioffsetFromVertical * 0.5);
    doc.drawContents(pPainter);
    pPainter->restore(); // Восстанавливаем состояние QPainter


//    // Центрирование подписи "Время (мин)" между графиками
    double timeLabelX = ioffsetFromSide + (VIRTUAL_SCREEN_WIDTH - ioffsetFromSide) / 2 - ioffsetFromSide / 2;
    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
    pPainter->drawText(timeLabelX, iOffsetYJs + fMachineRowCount * iMachineHeight + ioffsetFromVertical*0.5, "Время смены");


    // Отрисовка подписей "Время (мин)" с учетом динамического отступа
    double labelPositionY = iOffsetYMs + fJobRowCount * iMachineHeight + ioffsetFromVertical*0.5;
    double availableSpaceForLabel = VIRTUAL_SCREEN_HEIGHT - labelPositionY;

    // Проверяем, достаточно ли места для отступа снизу
    if (availableSpaceForLabel <   std::max((int)ioffsetFromVertical, 20)) {
        labelPositionY = VIRTUAL_SCREEN_HEIGHT - std::max((int)ioffsetFromVertical, 20);  // Поднимаем подпись вверх, чтобы обеспечить отступ снизу
    }

    pPainter->drawText(timeLabelX, labelPositionY, "Время смены");

    double timeLabelYBottom =iOffsetYMs + fJobRowCount * iMachineHeight + ioffsetFromVertical;
    // Убедимся, что подпись не заезжает на график
    if (timeLabelYBottom > VIRTUAL_SCREEN_HEIGHT - 10) { // Если подпись слишком низко, поднимаем выше
        timeLabelYBottom = VIRTUAL_SCREEN_HEIGHT - 10;
        pPainter->drawText(timeLabelX, timeLabelYBottom, "Время смены");
    }

     // рассчитываю где ось x для верхнего графика
    double iYPos_workers = iOffsetYJs + (fMachineRowCount-1 + 0.5) * iMachineHeight; // позиция для оси х по рабочим
    double iYPos_details = iOffsetYMs + (fJobRowCount-1 + 0.5) * iMachineHeight; // позиция для оси х по деталям



    pPainter->setFont(axisLabelFont);


    double lastDrawnPosition = -1;  // Переменная для хранения последней позиции, на которой был нарисован текст
    double step = 10;
    if (iMaxFinish > 100 && iMaxFinish <= 300) {
        step = 20;
    } else if (iMaxFinish > 300 && iMaxFinish <= 1000) {
        step = 50;
    } else if (iMaxFinish > 1000 && iMaxFinish <= 3000) {
        step = 150;
    } else if (iMaxFinish > 3000 && iMaxFinish <= 7000) {
        step = 300;
    } else if (iMaxFinish > 7000 && iMaxFinish <= 15000) {
        step = 500;
    }else if (iMaxFinish > 15000) {
        step = 1000;
    }
    for (double i = 0; i <= iMaxFinish; i += step) {
        double currentPosition = ioffsetFromSide + i * iScaleFactorX;

        // Рассчитываем ширину текста для текущего значения
        double textWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(i));

        // Если разница между текущей позицией и последней позицией меньше ширины текста, пропускаем отрисовку
        if (lastDrawnPosition != -1 && (currentPosition - lastDrawnPosition) < textWidth) {
            i += step;  // Пропускаем одну метку
            continue;
        }

        // Рисуем линии и текст
        pPainter->setPen(gridPen);
        pPainter->drawLine(currentPosition, iOffsetYJs, currentPosition, iYPos_workers);
        pPainter->drawLine(currentPosition, iOffsetYMs, currentPosition, iYPos_details);

        // Пересчитываем минуты в часы смены, начальное время - 8:00
        QTime baseTime(8, 0);
        QString resultTime = (baseTime.addSecs(i * 60)).toString("HH:mm");

        pPainter->setPen(textPen);
        pPainter->drawText(currentPosition - textWidth / 2, iOffsetYJs + fMachineRowCount * iMachineHeight + ioffsetFromVertical * 0.15, resultTime);
        pPainter->drawText(currentPosition - textWidth / 2, iOffsetYMs + fJobRowCount * iMachineHeight + ioffsetFromVertical * 0.15, resultTime);

        // Обновляем последнюю отрисованную позицию
        lastDrawnPosition = currentPosition;
    }

    // 1. Сохраняем информацию о выделенных машинах
    std::set<double> highlightedMachines;  // Набор для хранения выделенных машини
    highlightedMachines.clear();  // Очищаем набор выделенных машин
    std::set<double> highlightedJobs;  // Набор для хранения выделенных деталей
    highlightedJobs.clear();  // Очищаем набор выделенных деталей
    for (auto &sOp : m_vMsOperations_cont) {
        if (sOp.bHighlighted) {
            highlightedMachines.insert(sOp.iMachine);  // Добавляем выделенную машину в набор
            highlightedJobs.insert(sOp.iJob);  // Добавляем выделенную деталь в набор
        }
    }

    // 2. Отрисовка подписей для машин
    for (double i = 0; i < fMachineRowCount; ++i) {
        double iYPos = iOffsetYJs + (i + 0.5) * iMachineHeight;
        pPainter->setPen(gridPen);
        pPainter->drawLine(ioffsetFromSide, iYPos, VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iYPos);

        QPen resourcePen;
        std::cout << "[DEBUG] Accessing GetResources()[" << i << "], size = " << g_pSolver->GetResources().size() << std::endl;
        std::string resourceType = g_pSolver->GetResources()[i]->GetType();

        if ( resourceType == "Станок" )
            resourcePen = resourcePen1;
        else if ( resourceType == "Рабочий" )
            resourcePen = resourcePen2;
        else if ( resourceType == "Оснастка" )
            resourcePen = resourcePen3;
        else
            resourcePen = textPen;

        // Проверяем, выделена ли текущая машина
//        if (highlightedMachines.count(i + 1)) {
        if (highlightedMachines.count(i )) {
            QFont boldFont = pPainter->font();
            boldFont.setBold(true);  // Устанавливаем жирный шрифт
            pPainter->setFont(boldFont);
            pPainter->setPen(Qt::black);  // Устанавливаем черный цвет текста

            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1))-2 , iYPos - pPainter->fontMetrics().height() / 2 , pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1)) , pPainter->fontMetrics().height() );
//            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i ))-2 , iYPos - pPainter->fontMetrics().height() / 2 , pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i )) , pPainter->fontMetrics().height() );

            pPainter->setBrush(QBrush(Qt::yellow));  // Fill rectangle with yellow
            pPainter->setPen(Qt::yellow);
            pPainter->drawRect(rect);
            pPainter->setBrush(Qt::NoBrush);  // Reset brush
            //pPainter->setPen(Qt::black);  // Set black text color for label
            pPainter->setPen(resourcePen);
        } else {
            pPainter->setFont(verticalFont);  // Стандартный шрифт
            //pPainter->setPen(textPen);  // Стандартный цвет текста
            pPainter->setPen(resourcePen);
        }

        // Отрисовка подписи
        pPainter->drawText(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i + 1))-1.5, iYPos + 5, QString("Р %1").arg(i + 1));
//        pPainter->drawText(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Р %1").arg(i ))-1.5, iYPos + 5, QString("Р %1").arg(i ));
    }

    // 2. Отрисовка подписей для деталей
    for (double i = 0; i < fJobRowCount; ++i) {
        double iYPos = iOffsetYMs + (i + 0.5) * iMachineHeight;
        pPainter->setPen(gridPen);
        pPainter->drawLine(ioffsetFromSide, iYPos, VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iYPos);

        // Проверяем, выделена ли текущая деталь
//        if (highlightedJobs.count(i + 1)) {
        if (highlightedJobs.count(i )) {
            QFont boldFont = pPainter->font();
            boldFont.setBold(true);  // Устанавливаем жирный шрифт
            pPainter->setFont(boldFont);
            pPainter->setPen(Qt::black);  // Устанавливаем черный цвет текста

            // Draw yellow rectangle around the label to highlight it
            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)) - 2, iYPos - pPainter->fontMetrics().height() / 2 , pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i + 1)) , pPainter->fontMetrics().height() );
//            QRect rect(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i )) - 2, iYPos - pPainter->fontMetrics().height() / 2 , pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i )) , pPainter->fontMetrics().height() );

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
//        pPainter->drawText(ioffsetFromSide - pPainter->fontMetrics().horizontalAdvance(QString("Д %1").arg(i ))-1.5, iYPos + 5, QString("Д %1").arg(i ));


    }

    pPainter->setPen(gridPen);
    // Create a border for the top Gantt chart (machine chart)
    pPainter->drawLine(ioffsetFromSide, iOffsetYJs, VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iOffsetYJs);  // Top border
    pPainter->drawLine(VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iOffsetYJs, VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iYPos_workers);  // Right border
    // Create a border for the bottom Gantt chart (job chart)
    pPainter->drawLine(ioffsetFromSide, iOffsetYMs, VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iOffsetYMs);  // Top border for the job chart
    pPainter->drawLine(VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iOffsetYMs, VIRTUAL_SCREEN_WIDTH - ioffsetFromSide, iYPos_details);  // Right border for the job chart

    double distance = ioffsetFromSide / 2;  // Например, 1/3 от ширины текста

    QFontMetrics oMachineMetrics(pPainter->font());
    double iMachineTextHeight = oMachineMetrics.height();

    // Вертикальная подпись "Рабочие"
    pPainter->setFont(verticalFontRotate);
    pPainter->setPen(textPen);

    pPainter->save();
    double iMachineCenterY = (iOffsetYJs + iOffsetYJs + fMachineRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2 + distance / 2;
    pPainter->translate(ioffsetFromSide * 0.3 , iMachineCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Ресурсы");
    pPainter->restore();  // Восстанавливаем исходную систему координат

    // Вертикальная подпись "Детали"
    pPainter->setFont(verticalFontRotate);
    pPainter->setPen(textPen);

    pPainter->save();
    double iJobCenterY = (iOffsetYMs + iOffsetYMs + fJobRowCount * iMachineHeight) / 2 + iMachineTextHeight / 2 + distance / 2;
    pPainter->translate(ioffsetFromSide * 0.3 , iJobCenterY);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, "Работы");
    pPainter->restore();  // Восстанавливаем исходную систему координат

    // Отрисовка баров для операций на графике машин (m_vMsOperations_cont)
    for (auto &sOp : m_vMsOperations_cont) {
//        if (sOp.iMachine <= 0) {
        if (sOp.iMachine < 0 || sOp.iMachine >= fMachineRowCount) {
            qDebug() << "❗ Ошибка: sOp.iMachine < 0, данные:";
            qDebug() << "iStart:" << sOp.iStart << "iFinish:" << sOp.iFinish << "iJob:" << sOp.iJob << " sOp.iMachine :  " << sOp.iMachine << "fMachineRowCount: " << fMachineRowCount;
            continue;  // Пропускаем
        }
        double iBarStartX = ioffsetFromSide + sOp.iStart * iScaleFactorX;
        double iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        //add check for imachine
//        if (sOp.iMachine < 1 || sOp.iMachine > fMachineRowCount) {
        if (sOp.iMachine < 0 || sOp.iMachine >= fMachineRowCount) {
            qDebug() << "Ошибка: некорректный индекс машины: " << sOp.iMachine;
            continue;
        }

//        double iBarCenterY = iOffsetYJs + (sOp.iMachine - 1) * iMachineHeight + iMachineHeight / 2;
        double iBarCenterY = iOffsetYJs + (sOp.iMachine ) * iMachineHeight + iMachineHeight / 2;

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

        // Теперь добавляем штриховку/линию для части прямоугольника, равной setup_time
        double iSetupWidth =   sOp.iSetupTime  * iScaleFactorX; // Рассчитываем ширину для setup_time
        // Используем ту же область oMachineRect, но ограничиваем ширину setup_time
        pPainter->setBrush(Qt::NoBrush); // Отключаем заливку, чтобы не перекрывать предыдущую заливку

        // Устанавливаем полупрозрачное перо для наклонных линий
        QPen pen(QColor(0, 0, 0, 93)); // Полупрозрачный черный цвет (50% прозрачности)
        pPainter->setPen(pen);

        // Рисуем наклонные линии с шагом 1 пиксель
        for (double x = 0; x < iSetupWidth; x += 2) { // Шаг 2 пикселя, чтобы сделать наклонные линии
            double startX = iBarStartX + x;
            double startY = oMachineRect.top();
            double endX = iBarStartX + x -1; // Смещаем линию на один пиксель по диагонали
            double endY = oMachineRect.bottom();
            pPainter->drawLine(startX, startY, endX, endY); // Рисуем диагональную линию
        }

        // Рисуем контур и текст
        if (sOp.bHighlighted) {
            pPainter->drawRect(oMachineRect);
            dynamicFont.setBold(true);  // Делаем шрифт жирным
            pPainter->setFont(dynamicFont);
            pPainter->setPen(Qt::black); // Черный цвет текста
        } else {
            pPainter->drawRect(oMachineRect);
            pPainter->setFont(dynamicFont);
        }

        // Формируем текст для подписи
//        QString labelText = QString("Д %1 (%2)").arg(sOp.iJob).arg(QString::fromStdString(g_pSolver->GetOperations()[sOp.iOperation]->GetID()));
        QString labelText = QString("Д %1 (%2)").arg(sOp.iJob+1).arg(QString::fromStdString(g_pSolver->GetOperations()[sOp.iOperation]->GetID()));
        pPainter->drawText(oMachineRect, Qt::AlignCenter, labelText);
    }

    // Отрисовка баров для операций на графике задач (m_vJsOperations_cont)
    for (auto &sOp : m_vJsOperations_cont) {
        double iBarStartX = ioffsetFromSide + sOp.iStart * iScaleFactorX;
        double iBarWidth = (sOp.iFinish - sOp.iStart) * iScaleFactorX;
        //add check for ijob
//        if (sOp.iJob < 1 || sOp.iJob > fJobRowCount) {
        if (sOp.iJob < 0 || sOp.iJob >= fJobRowCount) {
            qDebug() << "Ошибка: некорректный индекс работы: " << sOp.iJob;
            continue;
        }

//        double iBarCenterY = iOffsetYMs + (sOp.iJob - 1) * iMachineHeight + iMachineHeight / 2;
        double iBarCenterY = iOffsetYMs + (sOp.iJob ) * iMachineHeight + iMachineHeight / 2;

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
//            labelText += QString::number(sOp.vMachinesIndexes[i]);
             labelText += QString::number(sOp.vMachinesIndexes[i]+1);
            if (i != sOp.vMachinesIndexes.size() - 1) {
                labelText += ", ";  // Добавляем запятую между номерами машин, кроме последнего
            }
        }

        pPainter->drawText(oJobRect, Qt::AlignCenter, labelText);
    }

    // Очистка данных недоступности
    vUnavData.clear();

    // Итерируемся по ресурсам с их индексами
    for (size_t resourceIndex = 0; resourceIndex < m_vResources.size(); ++resourceIndex) {
        const auto* pResource = m_vResources[resourceIndex];
        if (!pResource) continue; // Пропускаем, если ресурс отсутствует

        //add check for empty pResources
        if (m_vResources.empty()) {
            qDebug() << "Ошибка: m_vResources пуст!";
            return;
        }


        // Номер ресурса для отрисовки на графике
        int resourceRow = static_cast<int>(resourceIndex);

        // Получаем периоды недоступности ресурса
        const auto& vUnavailability = pResource->GetUnavailability();

        // Итерируемся по каждому периоду недоступности
        for (const auto& [unavStart, unavEnd] : vUnavailability) {
            // Вычисляем координаты для отрисовки периода недоступности
            double unavBarStartX = ioffsetFromSide + unavStart * iScaleFactorX;
            double unavBarWidth = (unavEnd - unavStart) * iScaleFactorX;
            double unavBarCenterY = iOffsetYJs + resourceRow * iMachineHeight + iMachineHeight / 2;

            // Создаем прямоугольник для периода недоступности
            QRect unavMachineRect(unavBarStartX, unavBarCenterY - iMachineHeight * 0.25, unavBarWidth, iMachineHeight * 0.5);

            // Добавляем данные в вектор
            vUnavData.push_back({static_cast<int>(resourceIndex+1), unavStart, unavEnd, unavMachineRect});
//            vUnavData.push_back({static_cast<int>(resourceIndex), unavStart, unavEnd, unavMachineRect});

            // Отрисовка прямоугольника с черным цветом
            pPainter->setPen(QPen(Qt::black, 3));  // Черный жирный контур
            pPainter->fillRect(unavMachineRect, Qt::black);  // Черная заливка
            pPainter->drawRect(unavMachineRect);
        }
    }

    // отрисовка красных линий
    QPen dashedLinePen(Qt::red, 2, Qt::DashLine);
    pPainter->setPen(dashedLinePen);

    std::vector<QPointF> predecessorGroupCenters;
    bool mainCenterValid = false;
    QPointF mainCenter;

for (size_t groupIdx = 0; groupIdx < m_vGroupedPredecessors.size(); ++groupIdx) {
    const auto& group = m_vGroupedPredecessors[groupIdx];
    if (group.empty()) {
        qDebug() << "Group" << groupIdx << "is empty. Skipping.";
        continue;
    }

    std::vector<SResourceOperation*> validGroup;
    for (auto* op : group) {
        if (op && op->iMachine > 0) {
//        if (op && op->iMachine >= 0) {
            validGroup.push_back(op);
        } else {
            qDebug() << "⚠️ Предшественник отброшен (nullptr или iMachine == 0):"
                     << "op =" << op
                     << (op ? QString("iMachine: %1, iStart: %2, iFinish: %3, iJob: %4")
                                     .arg(op->iMachine)
                                     .arg(op->iStart)
                                     .arg(op->iFinish)
                                     : "nullptr");
        }
    }


    if (validGroup.empty()) {
        qDebug() << "Group" << groupIdx << "has no valid operations after filtering. Skipping.";
        continue;
    }

    qDebug() << "Group" << groupIdx << "has" << validGroup.size() << "valid operations.";

    if (validGroup.size() == 1) {
        auto* op = validGroup[0];
        if (!op) {
            qDebug() << "Group" << groupIdx << "only valid op is nullptr. Skipping.";
            continue;
        }

        double x = ioffsetFromSide + op->iStart * iScaleFactorX +
                   (op->iFinish - op->iStart) * iScaleFactorX / 2;
//        double y = iOffsetYJs + (op->iMachine - 1 + 0.5) * iMachineHeight ;
        double y = iOffsetYJs + (op->iMachine  + 0.5) * iMachineHeight;
        qDebug() << "Group" << groupIdx << "single operation center at:" << QPointF(x, y);

        predecessorGroupCenters.emplace_back(x, y);
    } else {
        double x = ioffsetFromSide + validGroup[0]->iStart * iScaleFactorX +
                   (validGroup[0]->iFinish - validGroup[0]->iStart) * iScaleFactorX / 2;

        int minMachine = validGroup[0]->iMachine;
        int maxMachine = validGroup[0]->iMachine;
        for (auto* op : validGroup) {
            minMachine = std::min(minMachine, op->iMachine);
            maxMachine = std::max(maxMachine, op->iMachine);
        }



        double yStart = iOffsetYJs + (minMachine  + 0.5) * iMachineHeight;
        double yEnd   = iOffsetYJs + (maxMachine + 0.5) * iMachineHeight;

        qDebug() << "Group" << groupIdx << "vertical line at x =" << x
                 << "from y =" << yStart << "to y =" << yEnd;

        pPainter->drawLine(QPointF(x, yStart), QPointF(x, yEnd));

        predecessorGroupCenters.emplace_back(x, (yStart + yEnd) / 2);
    }
}



    // 2. Основная операция — вертикальная линия и координаты центра
    if (!m_vMainOperations.empty()) {
        std::vector<SResourceOperation*> validMainOps;
        for (auto* op : m_vMainOperations)
            if (op) validMainOps.push_back(op);

        if (!validMainOps.empty()) {
            double x = ioffsetFromSide + validMainOps[0]->iStart * iScaleFactorX +
                       (validMainOps[0]->iFinish - validMainOps[0]->iStart) * iScaleFactorX / 2;

            int minMachine = validMainOps[0]->iMachine;
            int maxMachine = validMainOps[0]->iMachine;
            for (auto* op : validMainOps) {
                minMachine = std::min(minMachine, op->iMachine);
                maxMachine = std::max(maxMachine, op->iMachine);
            }

//            double yStart = iOffsetYJs + (minMachine - 1 + 0.5) * iMachineHeight;
//            double yEnd   = iOffsetYJs + (maxMachine - 1 + 0.5) * iMachineHeight;

            double yStart = iOffsetYJs + (minMachine  + 0.5) * iMachineHeight;
            double yEnd   = iOffsetYJs + (maxMachine  + 0.5) * iMachineHeight;
            pPainter->drawLine(QPointF(x, yStart), QPointF(x, yEnd));

            mainCenter = QPointF(x, (yStart + yEnd) / 2);
            mainCenterValid = true;
        }
    }

    // 3. Стрелки от предшественников к основной операции
    if (mainCenterValid) {
        QPen arrowPen(Qt::red, 2);
        pPainter->setPen(arrowPen);
        const int arrowSize = 10;

        for (const auto& predCenter : predecessorGroupCenters) {
            QLineF line(predCenter, mainCenter);
            pPainter->drawLine(line);

            double angle = std::atan2(line.dy(), line.dx());
            QPointF arrowP1 = mainCenter - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                                   std::sin(angle + M_PI / 6) * arrowSize);
            QPointF arrowP2 = mainCenter - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                                   std::sin(angle - M_PI / 6) * arrowSize);

            QPolygonF arrowHead;
            arrowHead << mainCenter << arrowP1 << arrowP2;
            pPainter->drawPolygon(arrowHead);
        }
    }


}


void GanttChart::DrawWorkersTimeChart(QPainter *pPainter, int VIRTUAL_SCREEN_WIDTH, int VIRTUAL_SCREEN_HEIGHT) {
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
    int margin = std::min(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT) * 0.01;
    // Ширина для каждого бара
    int barWidth = (VIRTUAL_SCREEN_WIDTH - 20 * margin) / (numWorkers * 1.6);

    // Масштабные коэффициенты и отступы
    int marginHorizontal = std::max(static_cast<int>(VIRTUAL_SCREEN_WIDTH * 0.1), 30);
    int marginVertical = std::max(static_cast<int>(VIRTUAL_SCREEN_HEIGHT * 0.1), 50);
    int labelOffset = std::max(static_cast<int>(VIRTUAL_SCREEN_HEIGHT * 0.05), 20);

    // Динамический размер шрифта
    int fontSize = std::min(barWidth * 0.215, VIRTUAL_SCREEN_HEIGHT * 0.044); // std::min(barWidth * 0.15, VIRTUAL_SCREEN_HEIGHT * 0.02);
    QFont font = pPainter->font();
    font.setPointSize(fontSize);
    pPainter->setFont(font);

    int xAxisHeight = VIRTUAL_SCREEN_HEIGHT * 0.15;
    int offset = marginVertical * 0.1;
    int graphTop = marginVertical + labelOffset + offset;
    int graphBottom = VIRTUAL_SCREEN_HEIGHT - xAxisHeight - offset;
    int graphHeight = graphBottom - graphTop;
    int graphWidth = barWidth * numWorkers + 0.5 * barWidth + marginHorizontal * 0.2 * (numWorkers - 1);  // Скорректированная ширина оси X

    QString maxTimeLabel = QString::number(maxTime);
    QString maxWorkerLabel = QString("Р%1").arg(numWorkers);

    // Устанавливаем размер подписей по оси X, чтобы поместилось значение "Р + numWorkers"
    while ((graphWidth + 2 * marginHorizontal) > VIRTUAL_SCREEN_WIDTH && barWidth > 1) {
        barWidth -= 1;
        graphWidth = barWidth * numWorkers + 0.5 * barWidth + marginHorizontal * 0.2 * (numWorkers - 1);
    }

    // Обновляем шрифт для оси X, чтобы все значения были одинакового размера
    font.setBold(false);
    font.setWeight(QFont::Normal);
    pPainter->setFont(font);

    // Шрифт для заголовков и подписей по оси Y
    QFont titleFont = font;
    titleFont.setPointSize(font.pointSize() + 2);
    titleFont.setBold(false);
    titleFont.setWeight(QFont::Normal);

    // Отрисовка осей
    pPainter->drawLine(marginHorizontal, graphTop, marginHorizontal, graphBottom); // Ось Y слева
    pPainter->drawLine(marginHorizontal, graphBottom, marginHorizontal + graphWidth, graphBottom); // Ось X
    pPainter->drawLine(marginHorizontal + graphWidth , graphTop, marginHorizontal + graphWidth , graphBottom); // Вторая ось Y справа сдвинута на barWidth правее последней диаграммы

    // Подпись графика сверху
    titleFont.setBold(true);
    pPainter->setFont(titleFont);
    pPainter->drawText(marginHorizontal, marginVertical / 2 + offset, VIRTUAL_SCREEN_WIDTH - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Диаграмма утилизации ресурсов");
    titleFont.setBold(false);

    // Подписи по первой оси Y (Время в минутах)
    pPainter->setFont(font);
    for (int i = 0; i <= maxTime; i += (maxTime / 10)) {
        int y = graphBottom - (graphHeight * i / maxTime);
        pPainter->drawText(0, y - fontSize / 2, marginHorizontal - marginHorizontal/10, fontSize * 2, Qt::AlignRight, QString::number(i));
    }

    // Подпись по второй оси Y (Количество операций)
    if (maxJobCount > 0) {  // Проверка, чтобы избежать деления на ноль
        for (int i = 0; i <= maxJobCount; i += std::max(1, maxJobCount / 10)) {  // Используем std::max, чтобы шаг был не меньше 1
            int y = graphBottom - (graphHeight * i / maxJobCount);
            pPainter->drawText(marginHorizontal + graphWidth + marginHorizontal/10, y - fontSize / 2, marginHorizontal - marginHorizontal/10, fontSize * 2, Qt::AlignLeft, QString::number(i));
        }
    }

    // Поворот и подпись "Время (мин)" для оси Y
    pPainter->setFont(titleFont);
    int textWidth = pPainter->fontMetrics().horizontalAdvance("Время (мин)");
    int textHeight = pPainter->fontMetrics().height();
    pPainter->save();
    int textVerticalPosition = (VIRTUAL_SCREEN_HEIGHT / 2) + textWidth / 2 - offset;

    pPainter->translate(marginHorizontal*0.1, textVerticalPosition);
    pPainter->rotate(-90);
    pPainter->drawText(0, 0, textWidth, textHeight, Qt::AlignLeft, "Время (мин)");
    pPainter->restore();

    // Подпись "Количество работ" для правой оси Y с отступом
    pPainter->setFont(titleFont);
    int rightAxisTextWidth = pPainter->fontMetrics().horizontalAdvance("Количество работ");
    int rightAxisTextHeight = pPainter->fontMetrics().height();
    pPainter->save();
    pPainter->translate(VIRTUAL_SCREEN_WIDTH- marginHorizontal*0.2 , textVerticalPosition);

    pPainter->rotate(-90);
    pPainter->drawText(0, -rightAxisTextHeight / 0.5, rightAxisTextWidth, rightAxisTextHeight, Qt::AlignCenter, "Число операций");
    pPainter->restore();

    // Подпись "Номер рабочего" под осью X
    pPainter->setFont(titleFont);
    pPainter->drawText(marginHorizontal, VIRTUAL_SCREEN_HEIGHT - labelOffset - offset, VIRTUAL_SCREEN_WIDTH - 2 * marginHorizontal, labelOffset, Qt::AlignCenter, "Номер ресурса");

    // Отрисовка баров
    int xPosition = marginHorizontal+2;
    for (auto i = qmworkerTime.begin(); i != qmworkerTime.end(); ++i) {
        pPainter->setFont(font);
        int machineId = i.key();
        int time = i.value();
        int jobCount = qmworkerJobQount[machineId];

        int barHeightTime = graphHeight * time / maxTime;
        int barHeightJob = graphHeight * jobCount / maxJobCount;

        // Рисуем синий бар для времени
        QString label = QString("Р%1").arg(machineId);
        pPainter->setPen(Qt::black);
        pPainter->drawText(xPosition, graphBottom + labelOffset / 2 + offset, barWidth, labelOffset, Qt::AlignCenter, label);
        pPainter->fillRect(xPosition, graphBottom - barHeightTime, barWidth / 2 - 2, barHeightTime, Qt::blue);

        // Отображение значения времени над синим баром
        if (time > 0) {
            QFont timeFont = font;
            timeFont.setPointSize(fontSize - 2); // Установить размер шрифта на 2 пункта меньше, чем font
            pPainter->setFont(timeFont);

            int textHeight = pPainter->fontMetrics().height() ;  // Уменьшаем высоту текста в 2 раза
            int textWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(maxTime)); // Определяем ширину текста
            int yPosition = graphBottom - barHeightTime - marginVertical*0.65;  // Поднимаем текст немного выше
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

