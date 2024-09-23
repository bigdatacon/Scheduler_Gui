#ifndef JSONREADER_H
#define JSONREADER_H

#include <QString>
#include <vector>
#include <nlohmann/json.hpp>
#include "GanttChart.h"
#include <QDir>

struct MSOperation
{
    int jobIndex;
    int operationIndex;
    int startTime;
    int finishTime;
    int setupTime;
};
//struct JSOperation
//{
// int operationIndex;
// int machineIndex;
// int startTime;
// int finishTime;
//};

struct JSOperation
{
    std::vector<int> vMachinesIndexes;
    int operationIndex;
    int startTime;
    int finishTime;
};

class JsonReader {
public:
    QString getHigherPath(const QString &currentPath, int levelsUp) ;

    void serialize_operations(const std::vector<std::vector<MSOperation*>>& ms_operations, const std::vector<std::vector<JSOperation*>>& js_operations);
    void ReadOperationsFromFile(const QString &sFilename,std::vector<SJobOperation> &vJsOperations_cont, std::vector<SMachineOperation> &vMsOperations_cont /*std::vector<SOperation> &vJsOperations, std::vector<SOperation> &vMsOperations*/);
    void ReadOperationsFromFile_2(const QString &sFilename,std::vector<SJobOperation> &vJsOperations_cont, std::vector<SMachineOperation> &vMsOperations_cont /*std::vector<SOperation> &vJsOperations, std::vector<SOperation> &vMsOperations*/);

};

#endif // JSONREADER_H
