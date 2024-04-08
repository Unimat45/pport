cmake_minimum_required(VERSION 3.8)

project(pport++ VERSION 1.0)

option(BUILD_CLI "Build the command-line interface program" ON)
option(BUILD_SERVER "Build the server program" ON)

if (BUILD_CLI)
    add_subdirectory(cli)
endif()

if (BUILD_SERVER)
    add_subdirectory(server)
endif()
