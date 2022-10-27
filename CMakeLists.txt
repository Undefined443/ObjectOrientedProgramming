cmake_minimum_required(VERSION 3.23)
project(ObjectOrientedProgramming)

set(CMAKE_CXX_STANDARD 20)

add_executable(ObjectOrientedProgramming nlohmann/json.hpp main.cpp passenger.cpp passenger.h elevator.cpp elevator.h monitor.cpp monitor.h floor.cpp floor.h building.cpp building.h)
