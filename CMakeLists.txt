cmake_minimum_required(VERSION 3.14.0)
project(cmdp VERSION 1.0.0)

add_library(cmdp INTERFACE)

target_include_directories(cmdp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
