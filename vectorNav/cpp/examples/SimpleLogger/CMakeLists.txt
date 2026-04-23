cmake_minimum_required(VERSION 3.16)
project(SimpleLogger)
set(CMAKE_CXX_STANDARD 17)
set(CPP_ROOT ../..)

add_subdirectory(${CPP_ROOT} oVnSensor)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE oVnSensor)
target_link_libraries(${PROJECT_NAME} PRIVATE oVnSensor)

target_include_directories(${PROJECT_NAME} PRIVATE ${CPP_ROOT}/plugins/Logger)

message(STATUS "Built ${PROJECT_NAME}")