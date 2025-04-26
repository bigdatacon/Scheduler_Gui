#ifndef INPUTDATA_H
#define INPUTDATA_H

#pragma once

#include <string>
#include <vector>
#include <fstream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// Resource

class Resource
{
public:
    Resource(ifstream &file);
    ~Resource();

    string GetID() const;
    string GetName() const;
    string GetType() const;
    int GetCost() const;
    const vector<pair<string, int>>& GetUseCases() const;
    const vector<pair<int, int>>& GetUnavailability() const;
    bool IsRelevant(string sUseCase, int iDischarge) const;
    int AdjustStartTimeForOperation(int iStartTime, int iDuration) const;

    // void SaveToFileTXT(ofstream &file) const;

protected:
    string m_sID;
    string m_sName;
    string m_sType;
    int m_iCost;
    vector<pair<string, int>> m_vUseCases;      // pair<UseCase Discharge>
    vector<pair<int, int>> m_vUnavailability;   // pair<StartTime FinishTime>
                     
};

/////////////////////////////////////////////////////////////////////////////
// Operation

class Operation
{
public:
    Operation(ifstream &file, int iIndex);
    ~Operation();

    int GetIndex() const;
    string GetID() const;
    string GetType() const;
    int GetDuration() const;
    int GetDischarge() const;
    const vector<tuple<string, char, int>>& GetPredecessors() const;
    const vector<pair<string, int>>& GetRequiredResources() const;
    bool IsPredecessor(string sID, pair<char, int>* pRelationship = NULL) const;
    void SetRelevantResources(const vector<Resource*> &vAllResources);
    void GenerateRandSequencesOfResources(vector<vector<int>> &vvResourcesIndexes) const;

    // void SaveToFileTXT(ofstream &file) const;
   
protected:
    int m_iIndex;                                       // Самоиндексация (индекс для массива m_vOperations)
    string m_sID;
    string m_sType;
    int m_iDuration;
    int m_iDischarge;
    vector<tuple<string, char, int>> m_vPredecessors;   // tuple<ID, ConnectingPoint, Delay> 
    vector<pair<string, int>> m_vReqResources;          // pair<ResourceUseCase Duration>
    vector<vector<int>> m_vvRelResourcesIndexes; 
};

/////////////////////////////////////////////////////////////////////////////
// Job

class Job
{
public:
    Job(ifstream &file, int iIndex);
    ~Job();

    int GetIndex() const;
    string GetID() const;
    int GetDeadline() const;
    vector<string> GetOperationsIDs() const;
    void GetOperationsIndexes(const vector<Operation*> &vOperations, vector<int>* pOperationsIndexes) const;

    void SaveToFileTXT(ofstream &file) const;

protected:
    int m_iIndex;
    string m_sID;
    int m_iDeadline;
    vector<string> m_vOperationsIDs;   
};

#endif