#!/bin/bash

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null
then
    echo "clang-format could not be found. Please install clang-format first."
    exit 1
fi

# Define directories to exclude (e.g., build, third-party dependencies)
EXCLUDE_DIRS="build"

# Find all C++ source files, excluding directories defined in EXCLUDE_DIRS
FILES=$(find . -type f \( -iname "*.cpp" -o -iname "*.hpp" -o -iname "*.cxx" -o -iname "*.hxx" -o -iname "*.cc" -o -iname "*.h" \) \
    ! -path "./$EXCLUDE_DIRS/*")

if [ -z "$FILES" ]; then
    echo "No C++ source files found to format."
    exit 0
fi

# Run clang-format on each file found
echo "Formatting C++ files..."
for FILE in $FILES
do
    echo "Formatting: $FILE"
    clang-format -i "$FILE"
done

echo "Formatting completed!"