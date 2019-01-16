
cmake_minimum_required(VERSION 3.5)
project(logTest LANGUAGES CXX)

find_package(Threads REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -pedantic")
# -E -CC")
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/main.cpp)

add_executable(logTest ${SRC})

target_link_libraries(logTest logger ${CMAKE_THREAD_LIBS_INIT} )

