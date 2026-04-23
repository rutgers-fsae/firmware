cmake_minimum_required(VERSION 3.20)
project(FirmwareUpdate)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../plugins/FirmwareProgrammer FirmwareProgrammerPlugin)

message(STATUS "Build Firmware Update Project")

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE FirmwareProgrammerPlugin)
target_link_libraries(${PROJECT_NAME} PRIVATE FirmwareProgrammerPlugin)
