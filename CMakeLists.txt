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

CPMAddPackage("gh:fmtlib/fmt#10.1.1")
include_directories(${fmt_SOURCE_DIR}/include)
target_link_libraries(PlantUML2Cpp PUBLIC fmt::fmt)

CPMAddPackage("gh:CLIUtils/CLI11@2.3.2")
include_directories(${CLI11_SOURCE_DIR}/include)
target_link_libraries(PlantUML2Cpp PUBLIC CLI11::CLI11)

CPMAddPackage("gh:nlohmann/json@3.11.2")
include_directories(${json_SOURCE_DIR}/include)
target_link_libraries(PlantUML2Cpp PUBLIC nlohmann_json::nlohmann_json)

CPMAddPackage(
  GITHUB_REPOSITORY gabime/spdlog 
  VERSION 1.12.0
  OPTIONS "SPDLOG_FMT_EXTERNAL 1" 
)
include_directories(${spdlog_SOURCE_DIR}/include)
target_link_libraries(PlantUML2Cpp PUBLIC spdlog::spdlog)

add_subdirectory(test)
