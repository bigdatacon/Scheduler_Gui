#ifndef JSONREADER_H
#define JSONREADER_H

#include <QString>
#include <vector>
#include <nlohmann/json.hpp>
#include "GanttChart.h"

struct MSOperation
{
    int jobIndex;
    int operationIndex;
    int startTime;
    int finishTime;
    int setupTime;
};
struct JSOperation
{
 int operationIndex;
 int machineIndex;
 int startTime;
 int finishTime;
};

class JsonReader {
public:
    void serialize_operations(const std::vector<std::vector<MSOperation*>>& ms_operations, const std::vector<std::vector<JSOperation*>>& js_operations);
    void ReadOperationsFromFile(const QString &sFilename,std::vector<SJobOperation> &vJsOperations_cont, std::vector<SMachineOperation> &vMsOperations_cont /*std::vector<SOperation> &vJsOperations, std::vector<SOperation> &vMsOperations*/);
};

#endif // JSONREADER_H
