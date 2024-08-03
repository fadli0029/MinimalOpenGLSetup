#!/usr/bin/zsh

# Set the compiler to GCC
export CC=gcc
export CXX=g++

# Set the compiler to Clang
# export CC=clang
# export CXX=clang++

# If build directory does not exist, create it.
if [ ! -d "build" ]; then
    mkdir build
fi

# Check if CMakePresets.json exists in the project directory
if [ -f "CMakePresets.json" ]; then
    cmake --preset=default
    cmake --build build
else
    cmake -S . -B build
    cmake --build build
fi
