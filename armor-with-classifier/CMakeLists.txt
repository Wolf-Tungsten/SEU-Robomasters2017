cmake_minimum_required(VERSION 3.7)
project(armor_with_classifier)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)
set(SOURCE_FILES main.cpp armor_sample_marker.h)
add_executable(armor_with_classifier ${SOURCE_FILES})
target_link_libraries(armor_with_classifier ${OpenCV_LIBS})