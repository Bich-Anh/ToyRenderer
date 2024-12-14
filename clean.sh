#!/bin/bash
# clean.sh: Clean CMake build files

if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
else
    echo "No build directory found."
fi

echo "Recreating build directory and regenerating build files..."
mkdir build
cd build
cmake ..