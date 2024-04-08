cmake_minimum_required(VERSION 3.8)

project(argparse VERSION 1.0)

if (BUILD_SHARED_LIBS)
    add_library(argparse SHARED argparse.c)
else()
    add_library(argparse STATIC argparse.c)
endif()

target_include_directories(argparse PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

