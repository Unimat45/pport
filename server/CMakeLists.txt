cmake_minimum_required(VERSION 3.16)

project(pportd LANGUAGES C VERSION 2.0.0)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Type of build")
set(LOG ON CACHE BOOL "Should log queries")
set(CFG_FILE "pport.cfg" CACHE FILEPATH "Path of config file")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT UNIX)
    message(FATAL_ERROR "This program can only be compiled on linux")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/wsServer)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable(pportd ${SRC_FILES})

if (LOG)
    target_compile_definitions(pportd PRIVATE LOG)
endif()

target_compile_definitions(pportd PRIVATE CFG_FILE="${CFG_FILE}")
target_link_libraries(pportd PRIVATE ws)
target_include_directories(pportd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GNUInstallDirs)
install(TARGETS pportd
    EXPORT pportd-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
