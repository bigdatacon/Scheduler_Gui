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

    // чита данные из структур
    std::vector<std::vector<MSOperation*>> ms_operations = {
            {new MSOperation{1, 1, 0, 36, 0}, new MSOperation{1, 5, 144, 166, 0}},
            {new MSOperation{1, 2, 36, 64, 0}, new MSOperation{1, 3, 64, 107, 0}, new MSOperation{1, 4, 107, 144, 0}}

    };

    std::vector<std::vector<JSOperation*>> js_operations = {
            {new JSOperation{1, 1, 0, 36}, new JSOperation{2, 2, 36, 64}, new JSOperation{3, 2, 64, 107}, new JSOperation{4, 2, 107, 144}, new JSOperation{5, 1, 144, 166}}

    };

    // Сериализация данных для ms_operations
    for (int i = 0; i <  ms_operations.size(); ++ i) {
        for (const auto& op : ms_operations[i]) {
            m_vMsOperations_cont.push_back(SMachineOperation{
                                                 op->startTime,
                                                 op->finishTime,
                                                 op->jobIndex,
                                                 i+1
                                         });
        }
    }


    // Сериализация данных для js_operations
    for (int i = 0; i < js_operations.size(); ++i) {
        for (const auto& op : js_operations[i]) {
            m_vJsOperations_cont.push_back(SJobOperation{
                                                 op->startTime,
                                                 op->finishTime,
                                                 i+1,
                                                 op->machineIndex
                                         });
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
