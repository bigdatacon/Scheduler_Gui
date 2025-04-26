#ifndef OUTPUTDATA_H
#define OUTPUTDATA_H

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "InputData.h"

class SUnit;
class SOperation;
class SGroup;

/////////////////////////////////////////////////////////////////////////////
// SUnit

enum SUnitState { NOT_READY_TO_START, READY_TO_START, IN_SCHEDULE };
 
class SUnit
{
public:
    SUnit();
    virtual ~SUnit();

    virtual bool IsGroup() const = 0;
    virtual SUnitState GetState() const;
    virtual void UpdateState() = 0;
    virtual int GetStartTime() const;
    virtual void SetStartTime(int iStartTime);
    virtual int GetFinishTime() const;
    virtual void SetFinishTime(int iFinishTime);
    virtual int  GetSetupTime() const;
    virtual const vector<int>& GetResourcesIndexes() const;
    virtual void GenerateRandSequencesOfResources() = 0;
    virtual void SetToSchedule(vector<SUnit*> &vNewReadyToStartUnits);

    SUnitState m_eState;
    int m_iStartTime;
    int m_iFinishTime;
    int m_iSetupTime;
    vector<int> m_vResourcesIndexes;
    vector<vector<int>> m_vvRandResourcesIndexes;
protected:
    
    
    
};

/////////////////////////////////////////////////////////////////////////////
// SOperation

class SOperation : public SUnit 
{
public:
    SOperation(Operation* pOperation, Job* pJob);
    virtual ~SOperation();

    Operation* GetOperation() const;
    Job* GetJob() const;
    void AddPredecessor(SOperation* pSOperation, pair<char, int>& uRelationship);
    void AddFollower(SOperation* pSOperation);
    virtual bool IsGroup() const;
    virtual void UpdateState();
    virtual void GenerateRandSequencesOfResources();
    virtual void SetToSchedule(vector<SUnit*> &vNewReadyToStartUnits);

    void UpdateStartTime();
  
    Operation* m_pOperation;
    Job* m_pJob;
    SGroup* m_pSGroup;
protected:
    vector<tuple<SOperation*, char, int>> m_vPredecessors;
    vector<SOperation*> m_vFollowers;
};

/////////////////////////////////////////////////////////////////////////////
// SGroup

class SGroup : public SUnit 
{
public:
    SGroup();
    virtual ~SGroup();

    void AddOperation(SOperation* pSOperation);
    const vector<SOperation*>& GetOperations() const;
    virtual bool IsGroup() const;
    virtual void UpdateState();
    virtual void GenerateRandSequencesOfResources();
    virtual void SetToSchedule(vector<SUnit*> &vNewReadyToStartUnits);

//protected:  
    int m_iGroupIndex;
    int m_iDischarge;
    vector<SOperation*> m_vSOperations;
};

/////////////////////////////////////////////////////////////////////////////
// RSchedule

class RSchedule
{
public:
    RSchedule(int iResourcesCount);
    ~RSchedule();

    void AddUnit(SUnit* pSUnit);
    const vector<vector<SUnit*>>& GetUnits() const;
    int GetLastOperationIndex(int iResourceIndex) const;
    int GetFreeResourceTime(int iResourceIndex) const;

protected: 
    vector<vector<SUnit*>> m_vvSUnits;                // Назначенные операции
    vector<int> m_vFreeResourcesTimes;                // Времена, с которых ресурсы свободны
};

/////////////////////////////////////////////////////////////////////////////
// JSchedule

class JSchedule
{
public:
    JSchedule(int iJobsCount);
    ~JSchedule();

    void AddUnit(SUnit* pSUnit);
    const vector<vector<SOperation*>>& GetOperations() const;

protected:
    vector<vector<SOperation*>> m_vvSOperations;
};

/////////////////////////////////////////////////////////////////////////////
// Solution

class Solution
{
public:
    Solution(int iResourcesCount, int iJobsCount);
    ~Solution();

protected:
    int m_iObjective; 
    int m_iScheduleTime; 
    int m_iScheduleCost; 
    int m_iSumDeviation; 
    int m_iDeltaUtilization;
    int m_iFreeResourcesCount;
    RSchedule* m_pRSchedule;
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