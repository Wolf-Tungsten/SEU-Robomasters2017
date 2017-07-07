cmake_minimum_required(VERSION 3.7)
project(pyArmor)

set(CMAKE_CXX_STANDARD 11)
find_package( Boost REQUIRED COMPONENTS python)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
set(SOURCE_FILES pyArmor.cpp pyArmor.h)

add_library(pyArmor ${SOURCE_FILES})
target_link_libraries(pyArmor ${Boost_LIBRARIES})