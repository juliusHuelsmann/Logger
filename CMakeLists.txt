
cmake_minimum_required(VERSION 3.5)
project(logger VERSION 0.0.1.1 LANGUAGES CXX)




# Check for the ZMQ module. If found, build the code with ZMQ support.
find_package(PkgConfig REQUIRED)
find_package(Boost COMPONENTS filesystem system REQUIRED)
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
  if(DEFINED ZeroMQ_INCLUDE_DIR AND DEFINED ZeroMQ_LIBRARY 
      AND NOT ${ZeroMQ_INCLUDE_DIR} STREQUAL "ZeroMQ_INCLUDE_DIR-NOTFOUND" 
      AND NOT ${ZeroMQ_LIBRARY} STREQUAL "ZeroMQ_LIBRARY-NOTFOUND")

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


# Build the tests
OPTION(BuildTests "Build tests." OFF)
if (${BuildTests})
  add_subdirectory(test)
  add_subdirectory(examples)
endif()



# Build the libraries
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.hpp)
add_library(logger ${SRC})
target_compile_definitions(logger PUBLIC ${PUBLIC_COMPILE})
target_include_directories(logger PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(logger PRIVATE ${ZeroMQ_LIBRARY} PUBLIC pthread ${Boost_LIBRARIES})


set(CMAKE_INSTALL_PREFIX /usr/local/)
install(TARGETS logger LIBRARY DESTINATION lib/slog/ ARCHIVE DESTINATION lib/slog/)
file(GLOB_RECURSE SLOG_HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp ${PROJECT_SOURCE_DIR}/include/*.h)
message("headers: ${SLOG_HEADERS}")
#install(FILES ${SLOG_HEADERS} DESTINATION include/slog/)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/slog/)
