cmake_minimum_required(VERSION 3.16.0)

project(pport LANGUAGES C VERSION 4.2.0)

set(LOG ON CACHE BOOL "Should log queries")
set(CFG_FILE "pport.cfg" CACHE FILEPATH "Path of config file")

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

if (BUILD_SHARED_LIBS) 
    add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
else()
    add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (LOG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LOG)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE CFG_FILE="${CFG_FILE}")

