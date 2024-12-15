cmake_minimum_required(VERSION 3.16)
project(ToyRenderer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Enable compile commands generation (for Fleet)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the source directory
set(SOURCE_FILES src/main.cpp)

# Automatically find all subdirectories in the external directory
file(GLOB_RECURSE EXTERNAL_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/external/*/include)

# Define the output binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add the executable target
add_executable(ToyRenderer ${SOURCE_FILES})

# Add include directories for the target
target_include_directories(ToyRenderer
    PRIVATE ${PROJECT_SOURCE_DIR}/include       # Project-specific headers
    PRIVATE ${EXTERNAL_HEADERS_DIR}            # External headers
)