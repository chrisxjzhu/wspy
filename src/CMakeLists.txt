add_executable(wspy
               wspy.cpp
               net/url.cpp
               net/proxy.cpp
               net/http/headers.cpp
               net/http/request.cpp
               net/http/response.cpp
               net/http/client.cpp
               )
target_include_directories(wspy PRIVATE ./)
target_compile_options(wspy PRIVATE -Wall -Wextra -Wpointer-arith)
target_link_libraries(wspy lnx logging program_options progress_bar)
