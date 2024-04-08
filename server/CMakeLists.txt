cmake_minimum_required(VERSION 3.8)
project(pport++ LANGUAGES C CXX ASM_NASM VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Type of build")
set(CFG_FILE "pport_config.cfg" CACHE FILEPATH "Path of config file")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT UNIX)
    message(FATAL_ERROR "This program can only be compiled on linux")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/log.c")

file(GLOB CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(ASM_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/pio")

if(WIN32)
    set(ASM_FILE "${ASM_FILE}_w")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ASM_FILE "${ASM_FILE}_d")
endif()

set(ASM_FILE "${ASM_FILE}.s")
set_source_files_properties(${ASM_FILE} PROPERTIES LANGUAGE ASM_NASM)

add_executable(pportd ${CPP_FILES} ${ASM_FILE})

target_compile_definitions(pportd PRIVATE CFG_FILE="${CFG_FILE}")

target_link_libraries(pportd PRIVATE log.c)

if(WIN32)
    target_link_libraries(pportd PRIVATE Ws2_32.lib)
    target_link_options(pportd PRIVATE "/LARGEADDRESSAWARE:NO")
endif()

target_include_directories(pportd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

install(TARGETS pportd
    EXPORT pportd-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)