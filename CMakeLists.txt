# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(client_project LANGUAGES CXX)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find ZeroMQ Library (Manual Method) ---
# This method is more robust if find_package and pkg-config fail.
# It manually searches for the header and library files.

# Find the path to the ZeroMQ header file (zmq.hpp)
find_path(ZMQ_INCLUDE_DIR zmq.hpp
# Add common search paths. You might need to add your specific path here.
HINTS /usr/local/include /opt/local/include /usr/include
DOC "Path to the ZeroMQ header file (zmq.hpp)"
)

# Find the ZeroMQ library itself (e.g., libzmq.so or libzmq.a)
find_library(ZMQ_LIBRARY NAMES zmq libzmq
# Add common search paths for libraries.
HINTS /usr/local/lib /opt/local/lib /usr/lib
DOC "Path to the ZeroMQ library"
)

# Check if both the header and library were found
if(ZMQ_INCLUDE_DIR AND ZMQ_LIBRARY)
message(STATUS "Found ZeroMQ header in: ${ZMQ_INCLUDE_DIR}")
message(STATUS "Found ZeroMQ library at: ${ZMQ_LIBRARY}")
else()
# If not found, fail configuration with a helpful message.
message(FATAL_ERROR "Could not find ZeroMQ. Please ensure it is installed.\n"
"If it's in a non-standard location, you can help CMake find it by setting CMAKE_PREFIX_PATH.\n"
"Example: cmake -DCMAKE_PREFIX_PATH=/path/to/your/zeromq/install ..")
endif()


# Find the Threads library (for std::thread)
find_package(Threads REQUIRED)

# Add the executable target from your source file
add_executable(client_project main.cpp)

# Link the necessary libraries to your executable
# 1. Add the include directory for ZeroMQ headers
target_include_directories(client_project PRIVATE ${ZMQ_INCLUDE_DIR})

# 2. Link the ZeroMQ library and the Threads library
target_link_libraries(client_project PRIVATE ${ZMQ_LIBRARY} Threads::Threads)


# Print a message upon configuration
message(STATUS "Configured client_project. Use 'cmake --build .' to build.")
