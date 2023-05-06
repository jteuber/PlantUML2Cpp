cmake_minimum_required(VERSION 3.14)
project(PlantUML2Cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)
file(GLOB SRC_FILES
    "source/*.cpp"
    "source/PlantUml/*.cpp"
    "source/Cpp/*.cpp"
    "source/Cpp/*/*.cpp"
)
file(GLOB HEADER_FILES
    "include/*.h"
    "include/PlantUml/*.h"
    "include/Cpp/*.h"
    "include/Cpp/*/*.h"
)

add_executable(PlantUML2Cpp ${SRC_FILES} ${HEADER_FILES})

add_library(PlantUML2Cpp-static STATIC ${SRC_FILES})

# add dependencies
include(cmake/CPM.cmake)

CPMAddPackage("gh:TheLartians/PEGParser@2.2.0")
include_directories(${PEGParser_SOURCE_DIR}/include)
target_link_libraries(PlantUML2Cpp PUBLIC PEGParser)

CPMAddPackage("gh:fmtlib/fmt#7.1.3")
include_directories(${fmt_SOURCE_DIR}/include)
target_link_libraries(PlantUML2Cpp PUBLIC fmt::fmt)

add_subdirectory(test)
