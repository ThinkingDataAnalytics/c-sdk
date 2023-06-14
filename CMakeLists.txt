cmake_minimum_required(VERSION 3.12)
project(ta-c-sdk)
message(STATUS "[ThinkingEngine] CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM} ")

include_directories(include)

#################################################################

 # Product Library: logging consumer
  if(WIN32)
      add_compile_definitions(USE_WIN)
      set(CMAKE_C_FLAGS "-std=c89 -pedantic-errors -m64")
  else()
      add_compile_definitions(USE_POSIX)
      set(CMAKE_C_FLAGS "-std=c89")
  endif()
  SET(TE_LIB_NAME thinkingdata)
  add_library(${TE_LIB_NAME} src/thinkingdata.c src/json.c src/list.c src/util.c src/logging_consumer.c)
  if(WIN32)
      include_directories(thirdparty/pcre/include)
      link_directories(thirdparty/pcre/lib)
      target_link_libraries(${TE_LIB_NAME} pcre_x64)
  endif()

#################################################################

## Debug Library: debug consumer
#if(WIN32)
#    add_compile_definitions(USE_WIN)
#    set(CMAKE_C_FLAGS "-std=c99 -pedantic-errors -m64")
#else()
#    add_compile_definitions(USE_POSIX)
#    set(CMAKE_C_FLAGS "-std=c99")
#endif()
#SET(TE_LIB_NAME thinkingDataDebug)
#add_library(${TE_LIB_NAME} src/thinkingdata.c src/json.c src/list.c src/util.c src/debug_consumer.c src/http_client.c)
#if(WIN32)
#    add_compile_definitions(BUILDING_LIBCURL)
#    include_directories(thirdparty/pcre/include thirdparty/curl/include)
#	link_directories(thirdparty/pcre/lib thirdparty/curl/lib)
#    target_link_libraries(${TE_LIB_NAME} pcre_x64 libcurl)
#else()
#    target_link_libraries(${TE_LIB_NAME} curl)
#endif()

#################################################################

## Product Library: batch consumer
#if(WIN32)
#    add_compile_definitions(USE_WIN)
#    set(CMAKE_C_FLAGS "-std=c99 -pedantic-errors -m64")
#else()
#    add_compile_definitions(USE_POSIX)
#    set(CMAKE_C_FLAGS "-std=c99")
#endif()
# SET(TE_LIB_NAME thinkingDataBatch)
# add_library(${TE_LIB_NAME} src/thinkingdata.c src/json.c src/list.c src/util.c src/batch_consumer.c src/http_client.c)
# if(WIN32)
#     add_compile_definitions(BUILDING_LIBCURL)
#     include_directories(thirdparty/pcre/include thirdparty/curl/include)
# 	link_directories(thirdparty/pcre/lib thirdparty/curl/lib)
#     target_link_libraries(${TE_LIB_NAME} pcre_x64 libcurl)
# else()
#     target_link_libraries(${TE_LIB_NAME} curl)
# endif()

#################################################################
#
## Product Library: async batch consumer
#if(WIN32)
#    add_compile_definitions(USE_WIN)
#    set(CMAKE_C_FLAGS "-std=c99 -pedantic-errors -m64")
#else()
#    add_compile_definitions(USE_POSIX)
#    set(CMAKE_C_FLAGS "-std=c89")
#endif()
#SET(TE_LIB_NAME thinkingDataAsyncBatch)
#add_library(${TE_LIB_NAME} src/thinkingdata.c src/json.c src/list.c src/util.c src/async_batch_consumer.c src/http_client.c)
#if(WIN32)
#    add_compile_definitions(BUILDING_LIBCURL)
#    include_directories(thirdparty/pcre/include thirdparty/curl/include)
#	link_directories(thirdparty/pcre/lib thirdparty/curl/lib)
#    target_link_libraries(${TE_LIB_NAME} pcre_x64 libcurl)
#else()
#    target_link_libraries(${TE_LIB_NAME} curl)
#    find_package(Threads REQUIRED)
#    target_link_libraries(${TE_LIB_NAME} Threads::Threads)
#endif()

#################################################################

# create program execute file
add_executable(program demo/demo.c)
# link TE lib
target_link_libraries(program ${TE_LIB_NAME})

