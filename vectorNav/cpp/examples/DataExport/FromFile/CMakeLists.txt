cmake_minimum_required(VERSION 3.20)
project(DataExportFromFile)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/DataExport DataExportPlugin)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE DataExportPlugin)
target_link_libraries(${PROJECT_NAME} PRIVATE DataExportPlugin)

message(STATUS "Built ${PROJECT_NAME}.")
