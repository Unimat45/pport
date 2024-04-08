cmake_minimum_required(VERSION 3.8)

project(log.c LANGUAGES C VERSION 1.0)

option(BUILD_SHARED_LIBS "Build library as a shared object" OFF)

if (BUILD_SHARED_LIBS)
    add_library(log.c SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src/log.c")
else()
    add_library(log.c STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/log.c")
endif()

target_compile_definitions(log.c PRIVATE LOG_USE_COLOR)
target_include_directories(log.c PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
