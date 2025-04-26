#ifndef SOLVER_H
#define SOLVER_H

#pragma once

#include "InputData.h"
#include "OutputData.h"
#include <limits>

/////////////////////////////////////////////////////////////////////////////
// CIndividual

class CIndividual
{
public:
    CIndividual();
    ~CIndividual();

    void GenerateRandomGenotype(const vector<Resource*> &vResources, const vector<Operation*> &vOperations);

protected:
    vector<vector<int>> m_vvGenes;
};
 
/////////////////////////////////////////////////////////////////////////////
// Solver

struct Weights
{
    int iDuration;
    int iCost;
    int iSumSetupTime;
    int iSumDeviation;
    int iDeltaUtilization;
    int iFreeResourcesCount;
};

struct Modes    // Режимы применения эвристик: R - для выбора ресурсов (Этап 1), J - для выбора работ (Этап 2); значение (double, [0 1]) - вероятность применения режима
{ 
    double dR1; // Самая ранняя    
    double dR2; // Минимальная разница разрядов (Resource-operation)
    double dR3; // Осталось наибольшее число операций
    double dR4; // Требуется наибольшее время до завершения
    double dJ1; // Самая ранняя    
    double dJ2; // Минимальная разница разрядов (Resource-operation)
    double dJ3; // Осталось наибольшее число операций
    double dJ4; // Требуется наибольшее время до завершения
    double dJ5; // Текущая операция самая короткая
    double dJ6; // Текущая операция самая длинная
    double dJ7; // Минимум времени всех переналадок
};

class Solver
{
public:
    Solver();
    ~Solver();

    void SetIterationsCount(int iIterationsCount);
    void SetWeights(int iDuration, int iCost, int iSumSetupTime, int iSumDeviation, int iDeltaUtilization, int iFreeResourcesCount);
    void SetModes(double dR1, double dR2, double dR3, double dR4, double dJ1, double dJ2, double dJ3, double dJ4, double dJ5, double dJ6, double dJ7);

    const vector<Resource*>& GetResources() const;
    const vector<Operation*>& GetOperations() const;
    const vector<Job*>& GetJobs() const;
    
    RSchedule* GetRSchedule();
    JSchedule* GetJSchedule();
    ScheduleMetrics* GetScheduleMetrics();

    bool LoadProblemFromFileTXT(const char *filePath);

    void Run();
    
protected:
    int m_iIterationsCount;
    Weights m_Weights;
    Modes m_Modes;
    vector<Resource*> m_vResources;
    vector<Operation*> m_vOperations;
    vector<vector<string>> m_vvGroups;
    vector<Job*> m_vJobs;
    vector<vector<int>> m_vvSetupTimes;

    int m_iBestResult;
    RSchedule* m_pRSchedule;
    JSchedule* m_pJSchedule;
    ScheduleMetrics* m_pScheduleMetrics;

    void CalculateSchedule(const CIndividual &individual, RSchedule* pRSchedule, JSchedule* pJSchedule, ScheduleMetrics* pScheduleMetrics) const;
    bool CalculateResourcesForOperation_R1();

    int GetSetupTime(int nOperationIndex, RSchedule* pRSchedule, vector<int> vResourcesIndexes) const; // Переписать !!!
    bool SaveScheduleToFileTXT(const string &sFilePath, const RSchedule* pRSchedule, const JSchedule* pJSchedule) const;
};

#endif