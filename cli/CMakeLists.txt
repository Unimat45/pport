cmake_minimum_required(VERSION 3.8)
project(pportcli++ LANGUAGES C VERSION 1.0)

include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

set(SERV_ADDR "127.0.0.1" CACHE STRING "Address of the remote server")

file(GLOB CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/argparse")

add_executable(pport ${CPP_FILES})

target_compile_definitions(pport PRIVATE SERV_ADDR="${SERV_ADDR}")

target_include_directories(pport PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(pport PRIVATE argparse)

if (WIN32)
    target_link_libraries(pport PRIVATE Ws2_32.lib)
endif()

install(TARGETS pport
    EXPORT pport-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)