# Add executable from source files
add_executable(ToyRender main.cpp)

# Link the include directory to the target
target_include_directories(ToyRender PRIVATE ${PROJECT_SOURCE_DIR}/include)