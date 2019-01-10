cmake_minimum_required(VERSION 3.8)

project("linuxer")

add_compile_options(-std=c++14)

# ${CMAKE_CURRENT_LIST_DIR}
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src SRC_FILES)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/include
)

add_executable(xxx_main ${SRC_FILES})
target_link_libraries(xxx_main pthread)

message("SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# 添加线程库
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(xxx_main Threads::Threads)
endif ()

