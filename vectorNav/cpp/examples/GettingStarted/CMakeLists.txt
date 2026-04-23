cmake_minimum_required(VERSION 3.16)
project(GettingStarted)
set(CMAKE_CXX_STANDARD 17)
set(CPP_ROOT ../..)

set(THREADING NONE CACHE STRING "Enable threading")
set_property(CACHE THREADING PROPERTY STRINGS NONE ON OFF)

if(THREADING MATCHES NONE)
  message("Default")
elseif(THREADING)
  target_add_compile_definitions(${PROJECT_NAME} THREADING_ENABLE=true)
  message("Threading enabled")
else()
  target_add_compile_definitions(${PROJECT_NAME} THREADING_ENABLE=false)
  message("Threading disabled")
endif()

add_subdirectory(${CPP_ROOT} oVnSensor)

message(STATUS "Build ${PROJECT_NAME} target")
add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE oVnSensor)
target_link_libraries(${PROJECT_NAME} PRIVATE oVnSensor)

unset(THREADING CACHE)
