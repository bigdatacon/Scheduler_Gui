#ifndef JSONREADER_H
#define JSONREADER_H

#include <QString>
#include <vector>
#include <nlohmann/json.hpp>
#include "GanttChartLogic.h"

class JsonReader {
public:
    void ReadOperationsFromFile(const QString &sFilename, std::vector<SOperation> &vJsOperations, std::vector<SOperation> &vMsOperations);
};

#endif // JSONREADER_H
