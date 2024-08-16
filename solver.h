// solver.h
#ifndef SOLVER_H
#define SOLVER_H

struct SolverData {
    int x1;
    int x2;
    int x3;
};

struct result {
    int y1;
    int y2;
};

class Solver {
public:
    result solve(const SolverData& d) {
        result res;
        // Пример вычисления. Замените на свои формулы.
        res.y1 = d.x1 + d.x2 + d.x3;
        res.y2 = d.x1 * d.x2 * d.x3;
        return res;
    }
};

#endif // SOLVER_H
