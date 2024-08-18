cmake_minimum_required(VERSION 3.10)
project(SolverProgram CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(solver SHARED solver.cpp solver.h)

find_package(nlohmann_json REQUIRED)
target_link_libraries(solver PRIVATE nlohmann_json::nlohmann_json)

# Устанавливаем, чтобы библиотека имела расширение .so
set_target_properties(solver PROPERTIES
    OUTPUT_NAME "solver"
    SUFFIX ".so")
