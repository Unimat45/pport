cmake_minimum_required(VERSION 3.16)

project(pportd LANGUAGES C VERSION 2.0.0)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Type of build")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT UNIX)
    message(FATAL_ERROR "This program can only be compiled on linux")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/wsServer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE pport ws)

include(GNUInstallDirs)
install(TARGETS pportd
    EXPORT pportd-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
