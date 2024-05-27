# Add executables
add_executable(single_pipe single_pipe.cpp)
target_link_libraries(single_pipe PRIVATE phgasnets ${CERES_LIBRARIES} nlohmann_json::nlohmann_json HighFive)