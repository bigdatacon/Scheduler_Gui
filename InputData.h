#ifndef INPUTDATA_H
#define INPUTDATA_H

#pragma once

#include <string>
#include <vector>
#include <fstream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// Machine

class Machine
{
public:
    Machine();
    Machine(string sName, int nDischarge, int nCost, vector<string> vOperationsNames);
    ~Machine();

    string GetName() const;
    int GetDischarge() const;
    int GetCost() const;
    const vector<string>& GetOperationsNames() const;
    bool IsRelevant(string sOperationName, int nOperationDischarge) const;

    void SaveToFileTXT(ofstream &file) const;

protected:
    string m_sName;
    int m_nDischarge;
    int m_nCost;
    vector<string> m_vOperationsNames;

    // list<int> m_lsUnavailPeriods;
};

/////////////////////////////////////////////////////////////////////////////
// Operation

class Operation
{
public:
    Operation();
    Operation(int nID, string sName, int nDuration, int nDischarge);
    ~Operation();

    int GetID() const;
    string GetName() const;
    int GetDuration() const;
    int GetDischarge() const;
    void SetRelevantMachines(const vector<Machine*> &vAllMachines);
    void GetRandPermOfRelevantMachines(vector<int>* pMachinesIndexes) const;

    void SaveToFileTXT(ofstream &file) const;

protected:
    int m_nID;
    string m_sName;
    int m_nDuration;
    int m_nDischarge;
    // vector<int> m_vPrevOperations;      // IDs
    vector<int> m_vRelevantMachines;    // indexes
};

/////////////////////////////////////////////////////////////////////////////
// Job

class Job
{
public:
    Job();
    Job(string sName, int nDeadline, vector<int> vOperationsIDs);
    ~Job();

    string GetName() const;
    int GetDeadline() const;
    vector<int> GetOperationsIDs() const;
    void GetOperationsIndexes(const vector<Operation*> &vOperations, vector<int>* pOperationsIndexes) const;

    void SaveToFileTXT(ofstream &file) const;

protected:
    string m_sName;
    int m_nDeadline;
    vector<int> m_vOperationsIDs;
};

#endif
