cmake_minimum_required(VERSION 3.14)

add_library(thinkingdata src/thinkingdata.c src/json.c src/list.c src/util.c)
add_executable(sdk-demo demo/demo.c)
target_link_libraries(sdk-demo thinkingdata)
add_compile_definitions(USE_POSIX)
