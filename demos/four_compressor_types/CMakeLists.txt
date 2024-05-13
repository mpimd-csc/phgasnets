enable_language(C)

# Ceres
find_package(Ceres 2.0.0 REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# HDF5
find_package(HDF5 REQUIRED)

# HighFive
find_package(HighFive 2.8 REQUIRED)

# nlohmann_json
find_package(nlohmann_json 3.10 REQUIRED)

# Add executables
add_executable(four_compressor_types four_compressor_types.cpp)
target_link_libraries(four_compressor_types PRIVATE phgasnets ${CERES_LIBRARIES} nlohmann_json::nlohmann_json HighFive)
