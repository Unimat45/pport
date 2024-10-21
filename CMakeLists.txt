cmake_minimum_required(VERSION 3.16)

project(pport VERSION 2.0.0)

option(BUILD_SERVER "Build the server program" ON)

if (BUILD_SERVER)
    add_subdirectory(server)
endif()
