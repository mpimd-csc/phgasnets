cmake_minimum_required(VERSION 3.9)

# Set a name and a version number for your project:
project(phgasnets VERSION 0.0.1 LANGUAGES CXX)

# Set CMake policies for this project

# We allow <Package>_ROOT (env) variables for locating dependencies
cmake_policy(SET CMP0074 NEW)

# Initialize some default paths
include(GNUInstallDirs)

# Define the minimum C++ standard that is required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type specific flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fopenmp -march=native -mfma -fpic -ftree-vectorize")

# Compilation options
# Find external dependencies

# nlohmann_json
find_package(nlohmann_json 3.10 REQUIRED)

# Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# HDF5
enable_language(C)
find_package(HDF5 REQUIRED)

# HighFive
find_package(HighFive 2.8 REQUIRED)

# compile the library
add_subdirectory(src)

# compile the demos
add_subdirectory(demos)

# Add an alias target for use if this project is included as a subproject in another project
add_library(phgasnets::phgasnets ALIAS phgasnets)

# # This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)
