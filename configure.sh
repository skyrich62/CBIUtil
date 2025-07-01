#!/bin/bash

# Set compilers to use GCC 9.5.0
export CC=/usr/local/gcc-9.5.0/bin/gcc
export CXX=/usr/local/gcc-9.5.0/bin/g++

# Create a build directory
mkdir -p cmake-build
cd cmake-build

# Run CMake configuration with Ninja
cmake -G Ninja ..

