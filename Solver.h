#ifndef SOLVER_H
#define SOLVER_H

#pragma once

#include "InputData.h"
#include "OutputData.h"

/////////////////////////////////////////////////////////////////////////////
// CIndividual

class CIndividual
{
public:
    CIndividual();
    ~CIndividual();

    void GenerateRandomGenotype(const vector<Machine*> &vMachines, const vector<Operation*> &vOperations);

protected:
    vector<vector<int>> m_vvGenes;
};

/////////////////////////////////////////////////////////////////////////////
// Solver

struct Weights
{
    int time;
    int cost;
    int deviation;
    int deltaUtil;
    int freeMachines;
    int setups;
};

struct Modes
{
    int m1; // Самая ранняя
    int m2; // Минимальная разница разрядов (machine-operation)
    int m3; // Осталось наибольшее число операций
    int m4; // Требуется наибольшее время до завершения
    int m5; // Текущая операция самая короткая
    int m6; // Текущая операция самая длинная
    int m7; // Минимум времени всех переналадок
};

class Solver
{
public:
    Solver();
    ~Solver();

    void AddMachine(string sName, int nDischarge, int nCost, vector<string> vOperationsNames);
    void AddOperation(int nID, string sName, int nDuration, int nDischarge);
    void AddJob(string sName, int nDeadline, vector<int> vOperationsIDs);
    void SetIterationsCount(int nIterationsCount);
    void SetWeights(int time, int cost, int deviation, int deltaUtil, int freeMachines, int setups);
    void SetModes(int m1, int m2, int m3, int m4, int m5, int m6, int m7);

    MSchedule* GetMSchedule();
    JSchedule* GetJSchedule();

    bool LoadProblemFromFileTXT(const char *filePath);
    bool LoadProblemFromFileJSON(const char *fileName);

    void Run();

protected:
    int m_nIterationsCount;
    Weights m_Weights;
    Modes m_Modes;
    vector<Machine*> m_vMachines;
    vector<Operation*> m_vOperations;
    vector<Job*> m_vJobs;
    vector<vector<int>> m_vvSetupTimes;

    MSchedule* m_pMSchedule;
    JSchedule* m_pJSchedule;

    int CalculateSchedule(const CIndividual &individual, MSchedule* pMSchedule, JSchedule* pJSchedule) const;
    int GetSetupTime(int nOperationIndex, int nPrevOperationIndex) const;
    bool SaveScheduleToFileTXT(const string &sFilePath, const MSchedule* pMSchedule, const JSchedule* pJSchedule) const;
    bool SaveScheduleToFileJSON(const string &sFileName, const MSchedule* pMSchedule, const JSchedule* pJSchedule) const;
};

// Экспортируем функции
/*extern "C"
{
    __attribute__((visibility("default"))) void AddMachine(string sName, int nDischarge, int nCost, vector<string> vOperationsNames);
    __attribute__((visibility("default"))) void AddOperation(int nID, string sName, int nDuration, int nDischarge);
    __attribute__((visibility("default"))) void AddJob(string sName, int nDeadline, vector<int> vOperationsIDs);
    __attribute__((visibility("default"))) void SetIterationsCount(int nIterationsCount);
    __attribute__((visibility("default"))) void SetWeights(int time, int cost, int deviation, int deltaUtil, int freeMachines, int setups);
    __attribute__((visibility("default"))) void SetModes(int m1, int m2, int m3, int m4, int m5, int m6, int m7);
    __attribute__((visibility("default"))) void Run();
    __attribute__((visibility("default"))) MSchedule* GetMSchedule();
    __attribute__((visibility("default"))) JSchedule* GetJSchedule();
    __attribute__((visibility("default"))) bool LoadProblemFromFileJSON(const char *fileName);
}*/

#endif
