# Add executables
add_executable(two_pipe_compressor two_pipe_compressor.cpp)
target_link_libraries(two_pipe_compressor PRIVATE phgasnets ${CERES_LIBRARIES} nlohmann_json::nlohmann_json HighFive)
