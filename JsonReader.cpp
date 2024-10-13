#include "JsonReader.h"
#include <fstream>
#include <iostream>


QString JsonReader::getHigherPath(const QString &currentPath, int levelsUp) {
    QDir dir(currentPath);
    for (int i = 0; i < levelsUp; ++i) {
        dir.cdUp(); // Поднимаемся на один уровень вверх
    }
    return dir.absolutePath(); // Получаем полный путь
}


void JsonReader::ReadOperationsFromFile(const QString &sFilename, std::vector<SJobOperation> &m_vJsOperations_cont, std::vector<SMachineOperation> &m_vMsOperations_cont, ScheduleMetrics &m_ScheduleMetrics) {
    std::vector<std::vector<MSOperation*>> ms_operations = {
            {new MSOperation{2, 5, 0, 32 ,10}},
            {new MSOperation{}},
            {new MSOperation{3, 1, 40, 71, 12},  new MSOperation{2, 9, 72, 103, 13}},
            {new MSOperation{1, 6, 0, 28 ,23}},
            {new MSOperation{1, 2, 28, 33 ,3}},
            {new MSOperation{3, 10, 0, 40 ,7}},
            {new MSOperation{3, 3, 71, 99 ,4}},
            {new MSOperation{1, 6, 0, 28 ,2}},
            {new MSOperation{1, 7, 33, 65 ,10}},
            {new MSOperation{}},
            {new MSOperation{1, 6, 0, 28 ,9}},
            {new MSOperation{2, 8, 32, 72, 11},  new MSOperation{1, 4, 72, 134, 22}}
    };
    std::vector<std::vector<JSOperation*>> js_operations = {
        {new JSOperation{{4, 8, 11}, 6, 0, 28}, new JSOperation{{5}, 2, 28, 33}, new JSOperation{{9}, 7, 33, 65}, new JSOperation{{12}, 4, 72, 134}},
        {new JSOperation{{1}, 5, 0, 32}, new JSOperation{{12}, 8, 32, 72}, new JSOperation{{3}, 9, 72, 103}},
        {new JSOperation{{6}, 10, 0, 40}, new JSOperation{{3}, 1, 40, 71}, new JSOperation{{7}, 3, 71, 99}}
    };


    m_vMsOperations_cont.clear();
    m_vJsOperations_cont.clear();

    // Сериализация данных для ms_operations
    for (int i = 0; i < ms_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (ms_operations[i].empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями
            m_vMsOperations_cont.push_back(SMachineOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                0,      // Индекс работы по умолчанию
                i + 1,   // Машина (индекс +1)
                0,
                QRect(),    // Пустой QRect
                false       // Не выделен по умолчанию
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : ms_operations[i]) {
                m_vMsOperations_cont.push_back(SMachineOperation{
                    op->startTime,
                    op->finishTime,
                    op->jobIndex,
                    i + 1,
                    op->setupTime,
                    QRect(),    // Пустой QRect
                    false       // Не выделен по умолчанию
                });
            }
        }
    }


    // Сериализация данных для js_operations
    for (int i = 0; i < js_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (js_operations[i].empty() || js_operations[i][0]->vMachinesIndexes.empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями и пустым вектором индексов машин
            m_vJsOperations_cont.push_back(SJobOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                i + 1,  // Индекс работы (индекс +1)
                {},      // Пустой вектор машин
                QRect(),    // Пустой QRect
                false       // Не выделен по умолчанию
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : js_operations[i]) {
                m_vJsOperations_cont.push_back(SJobOperation{
                    op->startTime,
                    op->finishTime,
                    i + 1,
                    op->vMachinesIndexes,  // Переносим вектор индексов машин
                    QRect(),    // Пустой QRect
                    false       // Не выделен по умолчанию
                });
            }
        }
    }

    // Инициализирую структуры
    m_ScheduleMetrics.iScheduleTime = 100;
    m_ScheduleMetrics.iScheduleCost = 200;
    m_ScheduleMetrics.iSumDeviation = 300;
    m_ScheduleMetrics.iDeltaUtilization = 400;
    m_ScheduleMetrics.iFreeMachinesCount = 500;



    std::cout << "Machine Operations:\n";

    for (const auto& op : m_vMsOperations_cont) {

    std::cout << "Start: " << op.iStart

    << ", Finish: " << op.iFinish

    << ", Job: " << op.iJob

    << ", Machine: " << op.iMachine

    << '\n';

    }

    std::cout << "Job Operations:\n";

    for (const auto& op : m_vJsOperations_cont) {
        std::cout << "Start: " << op.iStart
                  << ", Finish: " << op.iFinish
                  << ", Job: " << op.iJob
                  << ", Machines: ";

        // Вывод всех индексов машин из вектора
        for (size_t j = 0; j < op.vMachinesIndexes.size(); ++j) {
            std::cout << op.vMachinesIndexes[j];
            if (j != op.vMachinesIndexes.size() - 1) {
                std::cout << ", "; // Добавляем запятую между индексами, кроме последнего
            }
        }

        std::cout << '\n'; // Переходим на новую строку после вывода каждой операции
    }


}

void JsonReader::ReadOperationsFromFile_2(const QString &sFilename, std::vector<SJobOperation> &m_vJsOperations_cont, std::vector<SMachineOperation> &m_vMsOperations_cont, ScheduleMetrics &m_ScheduleMetrics) {
    m_vMsOperations_cont.clear();
    m_vJsOperations_cont.clear();


    std::vector<std::vector<MSOperation*>> ms_operations = {
            {new MSOperation{1, 1, 71, 102, 3}, new MSOperation{1, 1, 169, 204, 4}}, // +1 к первому значению
            {new MSOperation{1, 2, 102, 129, 5}, new MSOperation{1, 2, 129, 169, 6}},
            {new MSOperation{3, 4, 0, 38, 7}, new MSOperation{1, 4, 38, 71, 8}, new MSOperation{2, 4, 71, 109, 9}, new MSOperation{3, 4, 109, 173, 10}},
            {new MSOperation{3, 5, 65, 107, 0}},
            {new MSOperation{2, 6, 14, 35, 0}, new MSOperation{3, 6, 38, 65, 0}, new MSOperation{3, 6, 227, 254, 0}},
            {new MSOperation{2, 7, 0, 14, 0}},
            {new MSOperation{2, 8, 109, 150, 0}, new MSOperation{3, 8, 173, 227, 0}}
    };

    std::vector<std::vector<JSOperation*>> js_operations = {
        {new JSOperation{{3}, 1, 38, 71}, new JSOperation{{1}, 1, 71, 102}, new JSOperation{{2}, 1, 102, 129}, new JSOperation{{2}, 1, 129, 169}, new JSOperation{{1}, 1, 169, 204}},  // +1 ко второму значению
        {new JSOperation{{6}, 2, 0, 14}, new JSOperation{{5}, 2, 14, 35}, new JSOperation{{3}, 2, 71, 109}, new JSOperation{{7}, 2, 109, 150}},  // +1 ко второму значению
        {new JSOperation{{3}, 3, 0, 38}, new JSOperation{{5}, 3, 38, 65}, new JSOperation{{4}, 3, 65, 107}, new JSOperation{{3}, 3, 109, 173}, new JSOperation{{7}, 3, 173, 227}, new JSOperation{{5}, 3, 227, 254}}  // +1 ко второму значению
    };

    // Сериализация данных для ms_operations
    for (int i = 0; i < ms_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (ms_operations[i].empty() ) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями
            m_vMsOperations_cont.push_back(SMachineOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                0,      // Индекс работы по умолчанию
                i + 1,   // Машина (индекс +1)
                0,
                QRect(),    // Пустой QRect
                false       // Не выделен по умолчанию
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : ms_operations[i]) {
                m_vMsOperations_cont.push_back(SMachineOperation{
                    op->startTime,
                    op->finishTime,
                    op->jobIndex,

                    i + 1,   // Машина (индекс +1)
                    op->setupTime,
                    QRect(),    // Пустой QRect
                    false       // Не выделен по умолчанию
                });
            }
        }
    }

    // Сериализация данных для js_operations
    for (int i = 0; i < js_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (js_operations[i].empty()|| js_operations[i][0]->vMachinesIndexes.empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями и пустым вектором индексов машин
            m_vJsOperations_cont.push_back(SJobOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                i + 1,  // Индекс работы (индекс +1)
                {},      // Пустой вектор машин
                QRect(),    // Пустой QRect
                false       // Не выделен по умолчанию
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : js_operations[i]) {
                m_vJsOperations_cont.push_back(SJobOperation{
                    op->startTime,
                    op->finishTime,
                    i + 1,
                    op->vMachinesIndexes,  // Переносим вектор индексов машин
                    QRect(),    // Пустой QRect
                    false       // Не выделен по умолчанию
                });
            }
        }
    }

    // Инициализирую структуры
    m_ScheduleMetrics.iScheduleTime = 100;
    m_ScheduleMetrics.iScheduleCost = 200;
    m_ScheduleMetrics.iSumDeviation = 300;
    m_ScheduleMetrics.iDeltaUtilization = 400;
    m_ScheduleMetrics.iFreeMachinesCount = 500;

}
