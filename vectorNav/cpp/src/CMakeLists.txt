cmake_minimum_required(VERSION 3.16)

set(SOURCES
    Interface/Sensor.cpp
    Interface/GenericCommand.cpp
    Interface/Registers.cpp
    Implementation/BinaryHeader.cpp
    Implementation/CommandProcessor.cpp
    Implementation/FaPacketProtocol.cpp
    Implementation/FbPacketProtocol.cpp
    Implementation/AsciiPacketProtocol.cpp
    Implementation/AsciiPacketDispatcher.cpp
    Implementation/FaPacketDispatcher.cpp
    Implementation/FbPacketDispatcher.cpp
    Implementation/PacketSynchronizer.cpp
)

message(STATUS "Build VnSensor")

if(NOT TARGET oVnSensor)
    add_library(oVnSensor OBJECT ${SOURCES})
endif()

if(NOT TARGET VnSensor)
    add_library(VnSensor SHARED ${SOURCES})
endif()

set_target_properties(VnSensor PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(UNIX)
    find_package(Threads)
    target_link_libraries(VnSensor ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(oVnSensor ${CMAKE_THREAD_LIBS_INIT})
endif(UNIX)

if(WIN32)
    target_link_libraries(VnSensor PUBLIC winmm)
    target_link_libraries(oVnSensor PUBLIC winmm)
endif()

target_include_directories(VnSensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(oVnSensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(VnSensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../plugins)
target_include_directories(oVnSensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../plugins)
