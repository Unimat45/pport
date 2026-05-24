cmake_minimum_required(VERSION 3.16)

project(pportd LANGUAGES C VERSION 2.0.0)

if(NOT UNIX OR NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "This program can only be compiled on Linux.")
endif()

set(CFG_FILE "pport.cfg" CACHE FILEPATH "Path of config file")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/wsServer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME}
    src/main.c
    src/config.c
    src/timings.c
)

if(PORT_ENABLE_LOGGING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LOG)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE CFG_FILE="${CFG_FILE}")
target_link_libraries(${PROJECT_NAME} PRIVATE pport ws)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
