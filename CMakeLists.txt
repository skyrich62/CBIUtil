cmake_minimum_required(VERSION 3.10)

project(CBIUtil VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/CompuBrite/*.cpp")

# Add the main library
add_library(CBIUtil STATIC ${SOURCES})

# Link pthread to the library
target_link_libraries(CBIUtil PRIVATE Threads::Threads)

# Link Google Test if available
find_library(GTEST_LIB gtest HINTS /usr/local/googleTest/lib)
find_library(GTEST_MAIN_LIB gtest_main HINTS /usr/local/googleTest/lib)

if(GTEST_LIB AND GTEST_MAIN_LIB)
    add_executable(CBIUtilTest test/test.cpp)
    target_link_libraries(CBIUtilTest CBIUtil ${GTEST_LIB} ${GTEST_MAIN_LIB} Threads::Threads)
else()
    message(WARNING "Google Test libraries not found. Test target will not be created.")
endif()

# Add definitions based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(CBIUtil PUBLIC CBI_CHECKPOINTS=1)
    target_compile_options(CBIUtil PUBLIC -g -O0 -Werror=return-type -Wno-deprecated-declarations)
else()
    target_compile_options(CBIUtil PUBLIC -O2 -Werror=return-type -Wno-deprecated-declarations)
endif()
