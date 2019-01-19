cmake_minimum_required(VERSION 3.8)

project("linuxer")

add_compile_options(-std=c++14)

option(CMAKE_DEMO "test demo.cpp" OFF)

if (CMAKE_DEMO)
    add_definitions(-DDEMO_FILE)
endif ()

option(CMAKE_MAIN "test main.cpp " OFF)

if (CMAKE_MAIN)
    add_definitions(-DMAIN_FILE)
endif ()

option(CMAKE_XHTTPD "test xhttpd.cpp " OFF)



# ${CMAKE_CURRENT_LIST_DIR}
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src SRC_FILES)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/demo SRC_FILES)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/include
)

if (CMAKE_XHTTPD)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/linux/web)
    add_definitions(-DXHTTPD)
    add_executable(xhttpd ${SRC_FILES} src/xhttpd_engine.cpp include/xhttpd_engine.h)
    target_link_libraries(xhttpd pthread)
endif ()

if (CMAKE_MAIN)
    # add_executable(xxx_main ${SRC_FILES})
    # target_link_libraries(xxx_main pthread)
endif ()

# target_link_libraries(xxx_main pthread)

message("SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# 添加线程库
#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    set(THREADS_PREFER_PTHREAD_FLAG ON)
#    find_package(Threads REQUIRED)
#    target_link_libraries(xxx_main Threads::Threads)
#endif ()

#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    target_link_libraries(xxx_main pthread)
#endif ()


