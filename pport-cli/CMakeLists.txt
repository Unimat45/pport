cmake_minimum_required(VERSION 3.0.0)
project(pport VERSION 1.0.0)

if (NOT UNIX)
	message("This app can only run on UNIX")
	return()
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(TARGET_SRC
	src/Arg.cpp
	src/Args.cpp
	src/main.cpp
	src/Client.cpp
)

add_executable(pport ${TARGET_SRC})

target_link_libraries(pport ${CMAKE_SOURCE_DIR}/lib/libjsoncpp.a)