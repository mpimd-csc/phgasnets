# Add executables
add_executable(four_compressor_types four_compressor_types.cpp)
target_link_libraries(four_compressor_types PRIVATE phgasnets ${CERES_LIBRARIES} nlohmann_json::nlohmann_json HighFive)
