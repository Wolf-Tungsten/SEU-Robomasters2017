cmake_minimum_required(VERSION 3.7)
project(armor_with_circle)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)
set(SOURCE_FILES main.cpp armor.h armor.cpp)
add_executable(armor_with_circle ${SOURCE_FILES})
target_link_libraries(armor_with_circle ${OpenCV_LIBS})