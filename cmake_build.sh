#!/bin/bash

# Helper script for building CBIUtil with CMake using the correct compiler

GCC_TOOLCHAIN="/usr/local/gcc-9.5.0"
PREFIX="/usr/local"

if [ "$1" = "debug" ] || [ "$1" = "Debug" ]; then
    BUILD_TYPE="Debug"
    BUILD_DIR="cmake-build-debug"
elif [ "$1" = "release" ] || [ "$1" = "Release" ]; then
    BUILD_TYPE="Release"  
    BUILD_DIR="cmake-build-release"
else
    echo "Usage: $0 {debug|release} [install]"
    echo "  debug|release: Build type"
    echo "  install: Optional - run make install after build"
    exit 1
fi

echo "Building $BUILD_TYPE version..."

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_CXX_COMPILER="$GCC_TOOLCHAIN/bin/g++" \
      -DCMAKE_C_COMPILER="$GCC_TOOLCHAIN/bin/gcc" \
      -DCMAKE_INSTALL_PREFIX="$PREFIX" \
      ..

# Build
make -j$(nproc)

# Install if requested
if [ "$2" = "install" ]; then
    echo "Installing to $PREFIX..."
    echo "Note: This may require elevated permissions"
    make install
fi

echo "$BUILD_TYPE build completed in $BUILD_DIR/"
