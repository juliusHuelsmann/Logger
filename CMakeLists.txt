
cmake_minimum_required(VERSION 3.5)
project(logger VERSION 0.0.1.1 LANGUAGES CXX)



# Check for the ZMQ module. If found, build the code with ZMQ support.
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

if (${PKG_CONFIG_FOUND})
  message("PkgConfig (optional):\tok")

  pkg_check_modules(PC_ZeroMQ QUIET zmq)
  find_path(ZeroMQ_INCLUDE_DIR
    NAMES zmq.hpp
    PATHS ${PC_ZeroMQ_INCLUDE_DIRS})
  find_library(ZeroMQ_LIBRARY
    NAMES zmq
    PATHS ${PC_ZeroMQ_LIBRARY_DIRS})
  if(DEFINED ZeroMQ_INCLUDE_DIR AND DEFINED ZeroMQ_LIBRARY)

    message("ZMQ (optional): \tok -- Building ${PROJECT_NAME} with ZMQ support!")
    set(PUBLIC_COMPILE FOUND_ZMQ="true")

  else()
    message("ZMQ (optional):  \tmissing -- Building ${PROJECT_NAME} WITHOUT ZMQ support!")
    set(ZeroMQ_LIBRARY "")
    set(ZeroMQ_INCLUDE_DIR "")
  endif()
else()
  message("PkgConfig (optional):\tmissing")
endif()

# Build the libraries
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(logger ${SRC})
target_compile_definitions(logger PUBLIC ${PUBLIC_COMPILE})
target_include_directories(logger PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(logger PRIVATE ${ZeroMQ_LIBRARY} pthread)

# Build the tests

OPTION(BUILD_TESTS "Build tests." OFF)
if (BUILD_TESTS)
  add_subdirectory(test)
  add_subdirectory(examples)
endif()
