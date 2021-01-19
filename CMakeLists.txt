cmake_minimum_required(VERSION 3.0.0)
project(PlantUML2Cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)
file(GLOB SRC_FILES
	"source/*.cpp"
)

add_executable(PlantUML2Cpp ${SRC_FILES})

add_library(PlantUML2Cpp-static STATIC ${SRC_FILES})

# add dependencies
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME PEGParser
  VERSION 2.0
  GIT_REPOSITORY https://github.com/TheLartians/PEGParser.git
)
link_directories(${PROJECT_BINARY_DIR}/_deps/pegparser-build)
include_directories(${PROJECT_BINARY_DIR}/_deps/pegparser-src/include)
target_link_libraries(PlantUML2Cpp PUBLIC PEGParser)

add_subdirectory(test)
