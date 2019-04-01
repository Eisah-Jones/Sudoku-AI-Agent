cmake_minimum_required(VERSION 3.6)
project(Sudoku_Cpp_Shell)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/BTSolver.cpp
        src/BTSolver.hpp
        src/Constraint.cpp
        src/Constraint.hpp
        src/ConstraintNetwork.cpp
        src/ConstraintNetwork.hpp
        src/Domain.cpp
        src/Domain.hpp
        src/Main.cpp
        src/SudokuBoard.cpp
        src/SudokuBoard.hpp
        src/Variable.cpp
        src/Variable.hpp
        src/Trail.cpp
        src/Trail.hpp)

add_executable(Sudoku_Cpp_Shell ${SOURCE_FILES})