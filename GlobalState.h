#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H

#include "Solver.h"

// Глобальная переменная для хранения состояния
enum State { DURATION, COST, SETUPS };
extern State g_currentState;

extern Solver* g_pSolver;
extern int g_iSolverPower;
extern int g_iR1;
extern int g_iJ3;

// Новая переменная для отслеживания статуса солвера
extern bool g_bSolverRunning;

#endif // GLOBALSTATE_H
