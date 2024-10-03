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
    void ReadOperationsFromFile(const QString &sFilename,std::vector<SJobOperation> &vJsOperations_cont, std::vector<SMachineOperation> &vMsOperations_cont , ScheduleMetrics &m_ScheduleMetrics);
    void ReadOperationsFromFile_2(const QString &sFilename,std::vector<SJobOperation> &vJsOperations_cont, std::vector<SMachineOperation> &vMsOperations_cont , ScheduleMetrics &m_ScheduleMetrics);

};

#endif // JSONREADER_H
