option(PHGASNETS_NUMERICDIFF "Enable numeric differentiation" OFF)

# Add executables
add_executable(four_compressor_types four_compressor_types.cpp)
target_link_libraries(four_compressor_types PRIVATE phgasnets ${CERES_LIBRARIES} nlohmann_json::nlohmann_json HighFive)

if(PHGASNETS_NUMERICDIFF)
    set_source_files_properties(four_compressor_types PROPERTIES COMPILE_DEFINITIONS "PHGASNETS_NUMERICDIFF=1")
else()
    set_source_files_properties(four_compressor_types PROPERTIES COMPILE_DEFINITIONS "PHGASNETS_NUMERICDIFF=0")
endif()