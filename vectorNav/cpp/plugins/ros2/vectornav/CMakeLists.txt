cmake_minimum_required(VERSION 3.5)
project(vectornav)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
# add_compile_options(-Wall -Wextra -Wpedantic)
# endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(vectornav_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# SDK
include_directories(../../../include)
include_directories(include)

message(STATUS "Including src")

# include_directories(../../../../VnSdk/cpp)
add_subdirectory(../../../src oVnSensor)

target_compile_definitions(oVnSensor PUBLIC GNSS_GROUP_ENABLE=0xFFFFFFFF)
target_compile_definitions(oVnSensor PUBLIC GNSS2_GROUP_ENABLE=0xFFFFFFFF)

# add_subdirectory(cpp)

# vectornav
add_executable(${PROJECT_NAME} src/vectornav.cc)
ament_target_dependencies(${PROJECT_NAME} rclcpp geometry_msgs rclcpp_action vectornav_msgs)
target_link_libraries(${PROJECT_NAME} oVnSensor)

# install
install(TARGETS
  ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)
install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(rosidl_default_runtime)
ament_package()

# install(DIRECTORY ../../../include/Core DESTINATION include)
# install(DIRECTORY ../../../include/Protocol DESTINATION include)
# install(DIRECTORY ../../../include/Implementation DESTINATION include)
# install(DIRECTORY ../../../include/HAL DESTINATION include)
