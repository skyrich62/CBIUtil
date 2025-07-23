cmake_minimum_required(VERSION 3.10)

project(CBIUtil VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set output directories to consolidate builds
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/CompuBrite/*.cpp")

# Add the main library (both static and shared)
add_library(CBIUtil_static STATIC ${SOURCES})
add_library(CBIUtil_shared SHARED ${SOURCES})

# Set output names based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(CBIUtil_static PROPERTIES OUTPUT_NAME "CBIUtil-d")
    set_target_properties(CBIUtil_shared PROPERTIES OUTPUT_NAME "CBIUtil-d")
else()
    set_target_properties(CBIUtil_static PROPERTIES OUTPUT_NAME "CBIUtil")
    set_target_properties(CBIUtil_shared PROPERTIES OUTPUT_NAME "CBIUtil")
endif()

# Create an alias for backward compatibility
add_library(CBIUtil ALIAS CBIUtil_static)

# Link pthread to both libraries
target_link_libraries(CBIUtil_static PRIVATE Threads::Threads)
target_link_libraries(CBIUtil_shared PRIVATE Threads::Threads)

# Link Google Test if available
find_library(GTEST_LIB gtest HINTS /usr/local/googleTest/lib)
find_library(GTEST_MAIN_LIB gtest_main HINTS /usr/local/googleTest/lib)

if(GTEST_LIB AND GTEST_MAIN_LIB)
    add_executable(CBIUtilTest test/test.cpp)
    target_link_libraries(CBIUtilTest CBIUtil_static ${GTEST_LIB} ${GTEST_MAIN_LIB} Threads::Threads)
else()
    message(WARNING "Google Test libraries not found. Test target will not be created.")
endif()

# Add definitions based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(CBIUtil_static PUBLIC CBI_CHECKPOINTS=1)
    target_compile_definitions(CBIUtil_shared PUBLIC CBI_CHECKPOINTS=1)
    target_compile_options(CBIUtil_static PUBLIC -g -O0 -Werror=return-type -Wno-deprecated-declarations -fPIC)
    target_compile_options(CBIUtil_shared PUBLIC -g -O0 -Werror=return-type -Wno-deprecated-declarations -fPIC)
else()
    target_compile_options(CBIUtil_static PUBLIC -O2 -Werror=return-type -Wno-deprecated-declarations -fPIC)
    target_compile_options(CBIUtil_shared PUBLIC -O2 -Werror=return-type -Wno-deprecated-declarations -fPIC)
endif()

# Installation configuration
# Install header files
install(DIRECTORY include/ DESTINATION include)

# Install libraries - CMake will use the OUTPUT_NAME property automatically
install(TARGETS CBIUtil_static CBIUtil_shared 
        ARCHIVE DESTINATION lib64
        LIBRARY DESTINATION lib64
        RUNTIME DESTINATION lib64)
