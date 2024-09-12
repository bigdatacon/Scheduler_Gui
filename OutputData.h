#ifndef OUTPUTDATA_H
#define OUTPUTDATA_H

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "InputData.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// MSchedule

struct MSOperation
{
    int jobIndex;
    int operationIndex;
    int startTime;
    int finishTime;
    int setupTime;
};

class MSchedule
{
public:
    MSchedule(int nMachinesCount);
    ~MSchedule();

    void SetOperation(int nMachineIndex, int nJobIndex, int nOperationIndex, int nStartTime, int nFinishTime, int nSetupTime);
    const vector<vector<MSOperation*>>& GetOperations() const;
    vector<MSOperation*> GetOperationsOnMachine(int nMachineIndex) const;
    int GetLastOperationIndex(int nMachineIndex) const;

    void SaveToFileTXT(ofstream &file) const;
    void SaveToFileJSON(const std::string& filename) const;

protected:
    vector<vector<MSOperation*>> m_vvMSOperations;
};

/////////////////////////////////////////////////////////////////////////////
// JSchedule

struct JSOperation
{
 int operationIndex;
 int machineIndex;
 int startTime;
 int finishTime;
};

class JSchedule
{
public:
    JSchedule(int nJobsCount);
    ~JSchedule();

    void SetOperation(int nJobIndex, int nOperationIndex, int nMachineIndex, int nStartTime, int nFinishTime);
    const vector<vector<JSOperation*>>& GetOperations() const;
    vector<JSOperation*> GetOperationsByJob(int nJobIndex) const;

    void SaveToFileTXT(ofstream &file) const;

protected:
    vector<vector<JSOperation*>> m_vvJSOperations;
};

/////////////////////////////////////////////////////////////////////////////
// Solution

class Solution
{
public:
    Solution(int nMachinesCount, int nJobsCount);
    ~Solution();

protected:
    int m_nObjective;
    int m_nScheduleTime;
    int m_nScheduleCost;
    int m_nSumDeviation;
    int m_nDeltaUtilization;
    int m_nFreeMachinesCount;
    MSchedule* m_pMSchedule;
    JSchedule* m_pJSchedule;
};

#endif
