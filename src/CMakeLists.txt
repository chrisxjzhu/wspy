add_executable(wspy wspy.cpp)
target_compile_options(wspy PRIVATE -Wall -Wextra -Wpointer-arith)
target_link_libraries(wspy lnx logging program_options progress_bar)
