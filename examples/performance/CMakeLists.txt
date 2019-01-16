cmake_minimum_required(VERSION 3.5)
project(performanceChecker CXX)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -pedantic -g")

add_executable(performanceChecker ${PROJECT_SOURCE_DIR}/main.cpp)

target_include_directories(performanceChecker PUBLIC 
  ${PROJECT_SOURCE_DIR}/../vendor/wrapper
  ${PROJECT_SOURCE_DIR}/../vendor/muParser)

target_link_libraries(performanceChecker logger muparser Wrapper::Wrapper)

