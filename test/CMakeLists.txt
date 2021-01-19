cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(tests VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include/)

add_executable(tests test.cpp)

# add dependencies
include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME Catch2
  GITHUB_REPOSITORY catchorg/Catch2
  VERSION 2.5.0
)
target_link_libraries(tests Catch2 PlantUML2Cpp-static PEGParser)

