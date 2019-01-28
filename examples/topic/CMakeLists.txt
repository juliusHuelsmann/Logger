
cmake_minimum_required(VERSION 3.5)
project(topicExample CXX)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} logger muparser Wrapper::Wrapper)

