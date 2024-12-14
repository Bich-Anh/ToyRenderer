cmake_minimum_required(VERSION 3.16)
project(ToyRender VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Enable compile commands generation (for Fleet)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include the src directory
add_subdirectory(src)

# Add include directory globally (optional)
include_directories(${PROJECT_SOURCE_DIR}/include)
