cmake_minimum_required(VERSION 3.16)
project(ToyRenderer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Enable compile commands generation (for Fleet)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the source directory
set(SOURCE_FILES src/main.cpp)

# Add include directory globally (optional)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Define the output binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(ToyRenderer ${SOURCE_FILES})