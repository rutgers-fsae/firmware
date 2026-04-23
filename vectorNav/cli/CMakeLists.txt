cmake_minimum_required(VERSION 3.16)
project("VnSdk_Net" VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
    message(FATAL_ERROR "This project requires MSVC compiler.")
endif()

add_subdirectory(src)
