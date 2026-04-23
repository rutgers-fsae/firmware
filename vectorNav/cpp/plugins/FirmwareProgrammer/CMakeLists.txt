cmake_minimum_required(VERSION 3.16)
project(FirmwareProgrammerPlugin)

set(CMAKE_CXX_STANDARD 17)

set(FIRMWARE_PLUGIN_SOURCES
    src/VnXml.cpp
    src/Bootloader.cpp
    src/FirmwareUpdater.cpp
)

set(CPP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_subdirectory(${CPP_ROOT} oVnSensor)

add_library(${PROJECT_NAME} STATIC ${FIRMWARE_PLUGIN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC oVnSensor)
target_link_libraries(${PROJECT_NAME} oVnSensor)
message(STATUS "Build ${PROJECT_NAME}")
