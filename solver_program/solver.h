#ifndef SOLVER_H
#define SOLVER_H

#include <string>

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
    result solve(const SolverData& d);
    SolverData readInputData(const std::string &inputFilePath);
    void writeOutputData(const result &res, const std::string &outputFilePath);
};

// Экспортируем функции
extern "C" {
    __attribute__((visibility("default"))) result solve(const SolverData& d);
    __attribute__((visibility("default"))) SolverData readInputData(const std::string &inputFilePath);
    __attribute__((visibility("default"))) void writeOutputData(const result &res, const std::string &outputFilePath);
}

#endif // SOLVER_H
