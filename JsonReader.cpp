#include "JsonReader.h"
#include <fstream>
#include <iostream>



//struct SJobOperation {
//    int iStart;
//    int iFinish;
//    int iJob;
//    int iMachine;
//};

//struct SMachineOperation {
//    int iStart;
//    int iFinish;
//    int iJob;
//    int iMachine;
//};

//std::vector<SJobOperation> m_vJsOperations_cont;
//std::vector<SMachineOperation> m_vMsOperations_cont;


//struct MSOperation
//{
//    int jobIndex;
//    int operationIndex;
//    int startTime;
//    int finishTime;
//    int setupTime;
//};
//struct JSOperation
//{
// int operationIndex;
// int machineIndex;
// int startTime;
// int finishTime;
//};





//// Функция для сериализации данных операций
//void JsonReader::serialize_operations(const std::vector<std::vector<MSOperation*>>& ms_operations, const std::vector<std::vector<JSOperation*>>& js_operations, ) {


//    // Сериализация данных для ms_operations
//    for (int i = 0; i <  ms_operations.size(); ++ i) {
//        for (const auto& op : ms_operations[i]) {
//            m_vMsOperations_cont.push_back({
//                                                 {"iStart", op->startTime},
//                                                 {"iFinish", op->finishTime},
//                                                 {"iJob", op->jobIndex},
//                                                 {"iMachine", i+1},
//                                         });
//        }
//    }


//    // Сериализация данных для js_operations
//    for (int i = 0; i < js_operations.size(); ++i) {
//        for (const auto& op : js_operations[i]) {
//            m_vJsOperations_cont.push_back({
//                                                 {"iStart", op->startTime},
//                                                 {"iFinish", op->finishTime},
//                                                 {"iJob", i+1},
//                                                 {"iMachine", op->machineIndex},
//                                         });
//        }
//    }

//}


QString JsonReader::getHigherPath(const QString &currentPath, int levelsUp) {
    QDir dir(currentPath);
    for (int i = 0; i < levelsUp; ++i) {
        dir.cdUp(); // Поднимаемся на один уровень вверх
    }
    return dir.absolutePath(); // Получаем полный путь
}


void JsonReader::ReadOperationsFromFile(const QString &sFilename, std::vector<SJobOperation> &m_vJsOperations_cont, std::vector<SMachineOperation> &m_vMsOperations_cont/*std::vector<SOperation> &vJsOperations, std::vector<SOperation> &vMsOperations*/) {
//    std::ifstream oFile(sFilename.toStdString());
//    nlohmann::json oJson;
//    oFile >> oJson;

//    // Читаем js_operations
//    for (const auto &item : oJson["js_operations"]) {
//        vJsOperations.push_back(SOperation{
//            item["Start"].get<int>(),
//            item["Finish"].get<int>(),
//            item["Job"].get<int>(),
//            item["Machine"].get<int>()
//        });
//    }

//    // Читаем ms_operations
//    for (const auto &item : oJson["ms_operations"]) {
//        vMsOperations.push_back(SOperation{
//            item["Start"].get<int>(),
//            item["Finish"].get<int>(),
//            item["Job"].get<int>(),
//            item["Machine"].get<int>()
//        });
//    }

//    // чита данные из структур
//    std::vector<std::vector<MSOperation*>> ms_operations = {
//            {new MSOperation{1, 1, 0, 36, 0}, new MSOperation{1, 5, 144, 166, 0}},
//            {new MSOperation{1, 2, 36, 64, 0}, new MSOperation{1, 3, 64, 107, 0}, new MSOperation{1, 4, 107, 144, 0}}

//    };

//    std::vector<std::vector<JSOperation*>> js_operations = {
//            {new JSOperation{1, 1, 0, 36}, new JSOperation{2, 2, 36, 64}, new JSOperation{3, 2, 64, 107}, new JSOperation{4, 2, 107, 144}, new JSOperation{5, 1, 144, 166}}

//    };

    std::vector<std::vector<MSOperation*>> ms_operations = {
            {new MSOperation{1, 13, 206, 215 ,0}},
            {new MSOperation{1, 1, 71, 102,0}, new MSOperation{1, 9, 102, 166,0}, new MSOperation{1, 5, 166, 206,0}},
            {new MSOperation{}},
            {new MSOperation{3, 10, 0, 38,0},  new MSOperation{1, 6, 38, 71,0},   new MSOperation{2, 14, 71,109,0}, new MSOperation{3,2,109,173,0}},
            {new MSOperation{3, 11, 65, 107,0}},
            {new MSOperation{2, 7, 14, 35,0},  new MSOperation{3, 3, 38, 65,0},   new MSOperation{3, 15, 217, 244,0}},
            {new MSOperation{3, 8, 173, 217,0}},
            {new MSOperation{2, 12, 0, 14,0},  new MSOperation{2, 4, 109, 165,0}}
    };
    std::vector<std::vector<JSOperation*>> js_operations = {
            {new JSOperation{6, 4, 38, 71}, new JSOperation{1, 2, 71, 102}, new JSOperation{9, 2, 102, 166}, new JSOperation{5, 2, 166, 206}, new JSOperation{13, 1, 206, 215}},
            {new JSOperation{12, 8, 0, 14}, new JSOperation{7, 6, 14, 35}, new JSOperation{14, 4, 71, 109}, new JSOperation{4, 8, 109, 165}},
            {new JSOperation{10, 4, 0, 38}, new JSOperation{3, 6, 38, 65}, new JSOperation{11, 5, 65, 107}, new JSOperation{2, 4, 109, 173}, new JSOperation{8, 7, 173, 217}, new JSOperation{15, 6, 217, 244}}
    };


    m_vMsOperations_cont.clear();
    m_vJsOperations_cont.clear();

//    // Сериализация данных для ms_operations
//    for (int i = 0; i <  ms_operations.size(); ++ i) {
//        for (const auto& op : ms_operations[i]) {
//            m_vMsOperations_cont.push_back(SMachineOperation{
//                                                 op->startTime,
//                                                 op->finishTime,
//                                                 op->jobIndex,
//                                                 i+1
//                                         });
//        }
//    }


//    // Сериализация данных для js_operations
//    for (int i = 0; i < js_operations.size(); ++i) {
//        for (const auto& op : js_operations[i]) {
//            m_vJsOperations_cont.push_back(SJobOperation{
//                                                 op->startTime,
//                                                 op->finishTime,
//                                                 i+1,
//                                                 op->machineIndex
//                                         });
//        }
//    }

    // Сериализация данных для ms_operations
    for (int i = 0; i < ms_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (ms_operations[i].empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями
            m_vMsOperations_cont.push_back(SMachineOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                0,      // Индекс работы по умолчанию
                i + 1   // Машина (индекс +1)
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : ms_operations[i]) {
                m_vMsOperations_cont.push_back(SMachineOperation{
                    op->startTime,
                    op->finishTime,
                    op->jobIndex+1,
                    i + 1
                });
            }
        }
    }

    // Сериализация данных для js_operations
    for (int i = 0; i < js_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (js_operations[i].empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями
            m_vJsOperations_cont.push_back(SJobOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                i + 1,  // Индекс работы (индекс +1)
                0       // Индекс машины по умолчанию
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : js_operations[i]) {
                m_vJsOperations_cont.push_back(SJobOperation{
                    op->startTime,
                    op->finishTime,
                    i + 1,
                    op->machineIndex+1
                });
            }
        }
    }


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

    << ", Machine: " << op.iMachine

    << '\n';

    }

}

void JsonReader::ReadOperationsFromFile_2(const QString &sFilename, std::vector<SJobOperation> &m_vJsOperations_cont, std::vector<SMachineOperation> &m_vMsOperations_cont/*std::vector<SOperation> &vJsOperations, std::vector<SOperation> &vMsOperations*/) {
    m_vMsOperations_cont.clear();
    m_vJsOperations_cont.clear();


    std::vector<std::vector<MSOperation*>> ms_operations = {
            {new MSOperation{1, 1, 71, 102, 0}, new MSOperation{1, 1, 169, 204, 0}}, // +1 к первому значению
            {new MSOperation{1, 2, 102, 129, 0}, new MSOperation{1, 2, 129, 169, 0}},
            {new MSOperation{3, 4, 0, 38, 0}, new MSOperation{1, 4, 38, 71, 0}, new MSOperation{2, 4, 71, 109, 0}, new MSOperation{3, 4, 109, 173, 0}},
            {new MSOperation{3, 5, 65, 107, 0}},
            {new MSOperation{2, 6, 14, 35, 0}, new MSOperation{3, 6, 38, 65, 0}, new MSOperation{3, 6, 227, 254, 0}},
            {new MSOperation{2, 7, 0, 14, 0}},
            {new MSOperation{2, 8, 109, 150, 0}, new MSOperation{3, 8, 173, 227, 0}}
    };
    std::vector<std::vector<JSOperation*>> js_operations = {
            {new JSOperation{1, 3, 38, 71}, new JSOperation{1, 1, 71, 102}, new JSOperation{1, 2, 102, 129}, new JSOperation{1, 2, 129, 169}, new JSOperation{1, 1, 169, 204}},  // +1 ко второму значению
            {new JSOperation{2, 6, 0, 14}, new JSOperation{2, 5, 14, 35}, new JSOperation{2, 3, 71, 109}, new JSOperation{2, 7, 109, 150}},  // +1 ко второму значению
            {new JSOperation{3, 3, 0, 38}, new JSOperation{3, 5, 38, 65}, new JSOperation{3, 4, 65, 107}, new JSOperation{3, 3, 109, 173}, new JSOperation{3, 7, 173, 227}, new JSOperation{3, 5, 227, 254}}  // +1 ко второму значению
    };

    // Сериализация данных для ms_operations
    for (int i = 0; i < ms_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (ms_operations[i].empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями
            m_vMsOperations_cont.push_back(SMachineOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                0,      // Индекс работы по умолчанию
                i + 1   // Машина (индекс +1)
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : ms_operations[i]) {
                m_vMsOperations_cont.push_back(SMachineOperation{
                    op->startTime,
                    op->finishTime,
                    op->jobIndex,
                    i + 1
                });
            }
        }
    }

    // Сериализация данных для js_operations
    for (int i = 0; i < js_operations.size(); ++i) {
        // Проверяем, пустой ли вектор операций
        if (js_operations[i].empty()) {
            // Если вектор пуст, добавляем операцию с нулевыми значениями
            m_vJsOperations_cont.push_back(SJobOperation{
                0,      // Старт по умолчанию
                0,      // Финиш по умолчанию
                i + 1,  // Индекс работы (индекс +1)
                0       // Индекс машины по умолчанию
            });
        } else {
            // Если операции есть, обрабатываем их
            for (const auto& op : js_operations[i]) {
                m_vJsOperations_cont.push_back(SJobOperation{
                    op->startTime,
                    op->finishTime,
                    i + 1,
                    op->machineIndex
                });
            }
        }
    }


}
