cmake_minimum_required(VERSION 3.14.0)
project(cmdp VERSION 1.0.0 LANGUAGES CXX)

# Header only library.
add_library(cmdp INTERFACE)
add_library(cmdp::cmdp ALIAS cmdp)

target_compile_features(cmdp INTERFACE cxx_std_11)

target_include_directories(cmdp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Only the top level project builds the tests.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    option(CMDP_BUILD_TESTS "Build the cmdp tests" ON)
else()
    option(CMDP_BUILD_TESTS "Build the cmdp tests" OFF)
endif()

if(CMDP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
