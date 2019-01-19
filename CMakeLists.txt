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

option(CMAKE_DEMO01 "test demo01.cpp " OFF)

if (CMAKE_DEMO01)
    add_definitions(-DDEMO01)
endif ()


# ${CMAKE_CURRENT_LIST_DIR}
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src SRC_FILES)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/demo SRC_FILES)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/include
)

add_executable(xxx_main ${SRC_FILES} src/p_c.cpp include/p_c.h demo/demo01.cpp)
# target_link_libraries(xxx_main pthread)

message("SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# 添加线程库
#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    set(THREADS_PREFER_PTHREAD_FLAG ON)
#    find_package(Threads REQUIRED)
#    target_link_libraries(xxx_main Threads::Threads)
#endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(xxx_main pthread)
endif ()

