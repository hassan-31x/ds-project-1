#!/bin/bash

# Create obj directory if it doesn't exist
mkdir -p obj

# Clean up previous build
rm -f class_scheduler
rm -f obj/*.o

# Detect OS for proper library paths
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS - use Homebrew paths
    RAYLIB_INCLUDE_PATH="/usr/local/opt/raylib/include"
    RAYLIB_LIB_PATH="/usr/local/opt/raylib/lib"
    
    # Compile all source files
    for file in src/*.cpp; do
        echo "Compiling $file..."
        g++ -c "$file" -o "obj/$(basename "${file%.*}").o" -std=c++11 -Wall -I./ -Isrc -I"$RAYLIB_INCLUDE_PATH"
    done
    
    # Link object files with raylib
    echo "Linking..."
    g++ obj/*.o -o class_scheduler -L"$RAYLIB_LIB_PATH" -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
    # Linux/Other
    # Compile all source files
    for file in src/*.cpp; do
        echo "Compiling $file..."
        g++ -c "$file" -o "obj/$(basename "${file%.*}").o" -std=c++11 -Wall -I./ -Isrc
    done
    
    # Link object files with raylib
    echo "Linking..."
    g++ obj/*.o -o class_scheduler -lraylib -lGL -lpthread -lm -ldl -lrt -lX11
fi

echo "Build complete! Run with ./class_scheduler" 