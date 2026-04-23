cmake_minimum_required(VERSION 3.16)
project(RegisterScanPlugin)

set(CMAKE_CXX_STANDARD 17)

set(CPP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_subdirectory(${CPP_ROOT} oVnSensor)

set(SOURCES
    src/RegisterScan.cpp 
    src/ConfigReader.cpp 
    src/ConfigWriter.cpp 
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CPP_ROOT}/libs oVnSensor)
target_link_libraries(${PROJECT_NAME} oVnSensor)
message(STATUS "Built ${PROJECT_NAME}")
