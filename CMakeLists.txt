cmake_minimum_required(VERSION 3.12)
project(ta-c-sdk)

if(WIN32)
    set(CMAKE_C_FLAGS "-std=c89")
else()
    add_compile_definitions(USE_POSIX)
    set(CMAKE_C_FLAGS "-std=c89 -Werror=declaration-after-statement")
endif()

include_directories(include)

add_library(thinkingdata src/thinkingdata.c src/json.c src/list.c src/util.c src/logging_consumer.c)
if(WIN32)
    include_directories(thirdparty/pcre/include)
	link_directories(thirdparty/pcre/lib)
    target_link_libraries(thinkingdata pcre_x86)
endif()

add_executable(sdk-demo demo/demo.c)
target_link_libraries(sdk-demo thinkingdata)

add_library(thinkingdata_http src/thinkingdata.c src/json.c src/list.c src/util.c src/batch_consumer.c src/http_client.c)
if(WIN32)
	add_compile_definitions(BUILDING_LIBCURL)
    include_directories(thirdparty/pcre/include thirdparty/curl/include)
	link_directories(thirdparty/pcre/lib thirdparty/curl/lib)
    #target_link_libraries(ta_sdk_http pcre_x86 libcurl ws2_32 winmm wldap32)
    target_link_libraries(thinkingdata_http pcre_x86 libcurl)
else ()
    target_link_libraries(thinkingdata_http curl)
endif()


add_executable(http_client_demo demo/http_client_demo.c)
target_link_libraries(http_client_demo thinkingdata_http)

#add_executable(sdk-demo demo/demo.c)
#target_link_libraries(sdk-demo thinkingdata_http)