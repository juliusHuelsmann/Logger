
cmake_minimum_required(VERSION 3.5)
project(logger VERSION 0.0.1.0 LANGUAGES CXX)

include_directories(include)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_library(logger ${SRC})
target_include_directories(logger PUBLIC include)

target_link_libraries(logger)


add_subdirectory(test)
