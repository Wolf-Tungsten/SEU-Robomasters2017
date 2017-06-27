cmake_minimum_required(VERSION 3.7)
project(armor)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

set(SOURCE_FILES main.cpp armor.cpp armor.h)
add_executable(armor ${SOURCE_FILES})
target_link_libraries(armor ${OpenCV_LIBS})