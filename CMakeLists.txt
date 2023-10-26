cmake_minimum_required(VERSION 3.12)
project(thinking_data_c)
message(STATUS "[ThinkingData] CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM} ")

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
  add_library(${TE_LIB_NAME} src/thinkingdata.c src/td_json.c src/td_list.c src/td_util.c src/td_logger_consumer.c)
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
#add_library(${TE_LIB_NAME} src/thinkingdata.c src/td_json.c src/td_list.c src/td_util.c src/td_debug_consumer.c src/td_http_client.c)
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
# add_library(${TE_LIB_NAME} src/thinkingdata.c src/td_json.c src/td_list.c src/td_util.c src/td_batch_consumer.c src/td_http_client.c)
# if(WIN32)
#     add_compile_definitions(BUILDING_LIBCURL)
#     include_directories(thirdparty/pcre/include thirdparty/curl/include)
# 	link_directories(thirdparty/pcre/lib thirdparty/curl/lib)
#     target_link_libraries(${TE_LIB_NAME} pcre_x64 libcurl)
# else()
#     target_link_libraries(${TE_LIB_NAME} curl)
# endif()

#################################################################

## Product Library: async batch consumer
#if(WIN32)
#    add_compile_definitions(USE_WIN)
#    set(CMAKE_C_FLAGS "-std=c99 -pedantic-errors -m64")
#else()
#    add_compile_definitions(USE_POSIX)
#    set(CMAKE_C_FLAGS "-std=c89")
#endif()
#SET(TE_LIB_NAME thinkingDataAsyncBatch)
#add_library(${TE_LIB_NAME} src/thinkingdata.c src/td_json.c src/td_list.c src/td_util.c src/td_async_batch_consumer.c src/td_http_client.c)
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

