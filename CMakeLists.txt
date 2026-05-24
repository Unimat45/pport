cmake_minimum_required(VERSION 3.16)

project(pport VERSION 2.0.0 LANGUAGES C)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_SERVER "Build the server program" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_link_options(-s)
endif()

if (BUILD_SERVER)
    add_subdirectory(server)
endif()
