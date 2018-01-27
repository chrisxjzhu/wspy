cmake_minimum_required(VERSION 3.6)
project(wspy)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Checking out dependent libraries...")
execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_subdirectory(libs/lnx)
add_subdirectory(libs/logging)
add_subdirectory(libs/program_options)
add_subdirectory(libs/progress_bar)
add_subdirectory(src)

add_custom_target(simple_test.sh ALL
                  DEPENDS test/simple_test.sh
                  COMMAND ${CMAKE_COMMAND} -E copy
                          ${PROJECT_SOURCE_DIR}/test/simple_test.sh
                          ${PROJECT_BINARY_DIR}/simple_test.sh
                  COMMENT "Copying simple_test.sh")

enable_testing()
add_test(simple_test simple_test.sh)
