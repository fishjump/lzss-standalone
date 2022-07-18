cmake_minimum_required(VERSION 3.22)

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME}
    PRIVATE
        circular_queue.c
        decode.c
        encode.c
        helper.c
)
