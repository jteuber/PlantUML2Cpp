cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(tests VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include/)

# add dependencies
include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.12.1
  VERSION 1.12.1
  OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
)


add_executable(tests main.cpp PlantUml/ParserTest.cpp
    Cpp/Class/TranslatorTest.cpp
    Cpp/Class/HeaderGeneratorTest.cpp
    Cpp/Class/IncludeGathererTest.cpp
    Cpp/Class/MemberSorterTest.cpp
    Cpp/Class/SourceGeneratorTest.cpp
    Cpp/Enum/TranslatorTest.cpp
    Cpp/Enum/HeaderGeneratorTest.cpp
    Cpp/Variant/TranslatorTest.cpp
    Cpp/Variant/HeaderGeneratorTest.cpp
    Common/ConfigTest.cpp)
target_link_libraries(tests gtest gtest_main gmock PlantUML2Cpp-static PEGParser fmt)

enable_testing()
add_test(tests tests)
