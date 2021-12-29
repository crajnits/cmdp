cmake_minimum_required(VERSION 3.14.0)

# Allow configuring this directory directly as well as from the top level.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(cmdp_test VERSION 1.0.0 LANGUAGES CXX)
    add_subdirectory(.. ${CMAKE_BINARY_DIR}/cmdp)
    enable_testing()
endif()

add_executable(cmdp_test
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdp_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdp_test_tu2.cpp
)

target_link_libraries(cmdp_test PRIVATE cmdp)

if(MSVC)
    target_compile_options(cmdp_test PRIVATE /W4)
else()
    target_compile_options(cmdp_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME cmdp_test COMMAND cmdp_test)
