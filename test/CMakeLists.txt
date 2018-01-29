macro(_add_test name)
    add_executable(${name} ${name}.cpp)
    target_compile_options(${name} PRIVATE -Wall -Wextra -Wpointer-arith)
    target_include_directories(${name} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/test)
    add_test(${name} ${name})
endmacro()

add_subdirectory(net)
