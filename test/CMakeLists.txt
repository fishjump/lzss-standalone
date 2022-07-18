cmake_minimum_required(VERSION 3.22)

add_executable(${PROJECT_TEST_NAME})

find_package(Threads REQUIRED)

target_include_directories(${PROJECT_TEST_NAME}
    PUBLIC
        ${GTEST_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_TEST_NAME}
    PRIVATE
        lzss_test.cpp
)

target_link_libraries(${PROJECT_TEST_NAME}
    PUBLIC
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_PROJECT_NAME}
)

add_test(
  NAME ${PROJECT_TEST_NAME}
  COMMAND ${PROJECT_TEST_NAME}
)
