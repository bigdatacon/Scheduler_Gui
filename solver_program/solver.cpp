#include "solver.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

SolverData Solver::readInputData(const std::string &inputFilePath) {
    SolverData data = {0, 0, 0};  // Инициализация данных по умолчанию
    std::ifstream inputFile(inputFilePath);

    std::cout << "[readInputData] Открытие файла: " << inputFilePath << std::endl;

    if (inputFile.is_open()) {
        json inputJson;
        inputFile >> inputJson;

        std::cout << "[readInputData] Прочитанный JSON: " << inputJson.dump(4) << std::endl;

        if (inputJson.contains("x1") && inputJson.contains("x2") && inputJson.contains("x3")) {
            data.x1 = inputJson["x1"].get<int>();
            data.x2 = inputJson["x2"].get<int>();
            data.x3 = inputJson["x3"].get<int>();

            std::cout << "[readInputData] Прочитанные данные: x1 = " << data.x1
                      << ", x2 = " << data.x2 << ", x3 = " << data.x3 << std::endl;
        } else {
            std::cerr << "[readInputData] Неверные данные в JSON файле" << std::endl;
        }
    } else {
        std::cerr << "[readInputData] Не удалось открыть файл" << std::endl;
    }

    inputFile.close();
    return data;
}

void Solver::writeOutputData(const result &res, const std::string &outputFilePath) {
    std::ofstream outputFile(outputFilePath);

    std::cout << "[writeOutputData] Открытие файла для записи: " << outputFilePath << std::endl;

    if (outputFile.is_open()) {
        json outputJson;
        outputJson["y1"] = res.y1;
        outputJson["y2"] = res.y2;

        std::cout << "[writeOutputData] Записываемые данные: y1 = " << res.y1
                  << ", y2 = " << res.y2 << std::endl;

        outputFile << outputJson.dump(4);  // Запись в формате JSON
    } else {
        std::cerr << "[writeOutputData] Не удалось открыть файл для записи" << std::endl;
    }

    outputFile.close();
}

result Solver::solve(const SolverData& d) {
    std::cout << "[solve] Полученные данные: x1 = " << d.x1
              << ", x2 = " << d.x2 << ", x3 = " << d.x3 << std::endl;

    result res = {0, 0};  // Инициализация значений res.y1 и res.y2
    res.y1 = d.x1 + d.x2 + d.x3;
    res.y2 = d.x1 * d.x2 * d.x3;

    std::cout << "[solve] Вычисленные результаты: y1 = " << res.y1
              << ", y2 = " << res.y2 << std::endl;

    return res;
}

// Экспортируем функции
extern "C" {
    result solve(const SolverData& d) {
        Solver solver;
        result res = solver.solve(d);
        std::cout << "[extern solve] Итоговые результаты: y1 = " << res.y1
                  << ", y2 = " << res.y2 << std::endl;
        return res;
    }

    SolverData readInputData(const std::string &inputFilePath) {
        Solver solver;
        SolverData data = solver.readInputData(inputFilePath);
        std::cout << "[extern readInputData] Итоговые прочитанные данные: x1 = " << data.x1
                  << ", x2 = " << data.x2 << ", x3 = " << data.x3 << std::endl;
        return data;
    }

    void writeOutputData(const result &res, const std::string &outputFilePath) {
        Solver solver;
        std::cout << "[extern writeOutputData] Запись итоговых данных: y1 = " << res.y1
                  << ", y2 = " << res.y2 << std::endl;
        solver.writeOutputData(res, outputFilePath);
    }
}
