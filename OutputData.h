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
    MSchedule(int nResourcesCount);
    ~MSchedule();

    void SetOperation(vector<int>& vResourcesIndexes, int nJobIndex, int nOperationIndex, int nStartTime, int nFinishTime, int nSetupTime);
    const vector<vector<MSOperation*>>& GetOperations() const;
    vector<MSOperation*> GetOperationsOnResource(int nResourceIndex) const;
    int GetLastOperationIndex(int nResourceIndex) const;

protected: 
    vector<vector<MSOperation*>> m_vvMSOperations;
};

/////////////////////////////////////////////////////////////////////////////
// JSchedule

struct JSOperation
{
    vector<int> vResourcesIndexes;
    int operationIndex;
    int startTime;
    int finishTime;
};

class JSchedule
{
public:
    JSchedule(int nJobsCount);
    ~JSchedule();

    void SetOperation(int nJobIndex, vector<int>& vResourcesIndexes, int nOperationIndex, int nStartTime, int nFinishTime);
    const vector<vector<JSOperation*>>& GetOperations() const;
    vector<JSOperation*> GetOperationsByJob(int nJobIndex) const;

protected:
    vector<vector<JSOperation*>> m_vvJSOperations;
};

/////////////////////////////////////////////////////////////////////////////
// Solution

class Solution
{
public:
    Solution(int nResourcesCount, int nJobsCount);
    ~Solution();

protected:
    int m_nObjective; 
    int m_nScheduleTime; 
    int m_nScheduleCost; 
    int m_nSumDeviation; 
    int m_nDeltaUtilization;
    int m_nFreeResourcesCount;
    MSchedule* m_pMSchedule;
    JSchedule* m_pJSchedule;
};

struct ScheduleMetrics
{
    int iDuration; 
    int iCost; 
    int iSumSetupTime;
    int iSumDeviation; 
    int iDeltaUtilization;
    int iFreeResourcesCount;
};

#endif