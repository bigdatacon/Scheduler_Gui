#include "GlobalState.h"

// Инициализация глобальной переменной
State g_currentState = DURATION;
Solver* g_pSolver = NULL;
int g_iSolverPower = 1;
int g_iR1 = 0;
int g_iJ3 = 0;
bool g_bSolverRunning = false;  // По умолчанию солвер не запущен
