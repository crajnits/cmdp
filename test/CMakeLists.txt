cmake_minimum_required(VERSION 3.14.0)
project(cmdp_test VERSION 1.0.0)

# Unit test executable
add_executable(cmdp_test
	${CMAKE_CURRENT_SOURCE_DIR}/cmdp_test.cpp
)

# cmdp lib
add_subdirectory(../ ${CMAKE_BINARY_DIR}/cmdp)

# link lib
target_link_libraries(cmdp_test
	PRIVATE cmdp
)
