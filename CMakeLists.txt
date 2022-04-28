cmake_minimum_required(VERSION 3.0.0)
project(pport VERSION 0.1.0)

include_directories(${CMAKE_SOURCE_DIR}/include)

# add_library(Args STATIC ${CMAKE_SOURCE_DIR}/include/Args.hpp ${CMAKE_SOURCE_DIR}/lib/Args.cpp)
# add_library(Arg STATIC ${CMAKE_SOURCE_DIR}/include/Arg.hpp ${CMAKE_SOURCE_DIR}/lib/Arg.cpp)

set(TARGET_SRC
	lib/Arg.cpp
	lib/Args.cpp
	src/main.cpp
)

add_executable(pport ${TARGET_SRC})

# target_link_libraries(pport PRIVATE Args)
# target_link_libraries(pport PRIVATE Arg)