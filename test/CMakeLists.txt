
cmake_minimum_required(VERSION 3.5)
project(logTest LANGUAGES CXX)

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/main.cpp)

add_executable(logTest ${SRC})

target_link_libraries(logTest logger)

