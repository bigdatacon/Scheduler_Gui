#include "JsonReader.h"
#include <fstream>

void JsonReader::ReadOperationsFromFile(const QString &sFilename, std::vector<SOperation> &vJsOperations, std::vector<SOperation> &vMsOperations) {
    std::ifstream oFile(sFilename.toStdString());
    nlohmann::json oJson;
    oFile >> oJson;

    // Читаем js_operations
    for (const auto &item : oJson["js_operations"]) {
        vJsOperations.push_back(SOperation{
            item["Start"].get<int>(),
            item["Finish"].get<int>(),
            item["Job"].get<int>(),
            item["Machine"].get<int>()
        });
    }

    // Читаем ms_operations
    for (const auto &item : oJson["ms_operations"]) {
        vMsOperations.push_back(SOperation{
            item["Start"].get<int>(),
            item["Finish"].get<int>(),
            item["Job"].get<int>(),
            item["Machine"].get<int>()
        });
    }
}
