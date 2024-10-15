#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H

// Глобальная переменная для хранения состояния
enum State { SETUP, WORKERS_TIME };
extern State g_currentState;

#endif // GLOBALSTATE_H
