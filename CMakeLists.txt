cmake_minimum_required(VERSION 3.22)

project(zmq_thread
	VERSION 1.0.0
	DESCRIPTION "A ZeroMQ node is constructed and run in a separate thread"
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ZMQ_BUILD_DRAFT_API 1)
set(ENABLE_DRAFTS ON)

set(CPPZMQ_BUILD_TESTS OFF)
set(ZMQ_HAVE_BUSY_POLL 0)

include(FetchContent)

FetchContent_Declare(ZeroMQ URL https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.tar.gz)
FetchContent_MakeAvailable(ZeroMQ)
FetchContent_GetProperties(ZeroMQ)
message(STATUS "ZeroMQ_POPULATED=${ZeroMQ_POPULATED}")
message(STATUS "ZeroMQ_SOURCE_DIR=${ZeroMQ_SOURCE_DIR}")
message(STATUS "ZeroMQ_BINARY_DIR=${ZeroMQ_BINARY_DIR}")

FetchContent_Declare(cppzmq URL https://github.com/zeromq/cppzmq/archive/refs/tags/v4.10.0.tar.gz)
FetchContent_MakeAvailable(cppzmq)
FetchContent_GetProperties(cppzmq)
message(STATUS "CPPZMQ_POPULATED=${cppzmq_POPULATED}")
message(STATUS "CPPZMQ_SOURCE_DIR=${cppzmq_SOURCE_DIR}")
message(STATUS "CPPZMQ_BINARY_DIR=${cppzmq_BINARY_DIR}")
message(STATUS "cppzmq_STATIC_LIBRARY=${cppzmq_STATIC_LIBRARY}")

##
## Library
##
add_library(zmqthread
            STATIC src/zmq_node.cpp
)
target_include_directories(
    zmqthread
    PRIVATE include
)
target_link_libraries(zmqthread
    PUBLIC cppzmq
)

##
## Application
##
add_executable(zmq_thread src/zmq_thread.cpp)
target_include_directories(zmq_thread
    PRIVATE include
)
target_link_libraries(zmq_thread
    PRIVATE zmqthread
            cppzmq
)