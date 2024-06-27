# Add executables
option(PHGASNETS_NUMERICDIFF "Enable numeric differentiation" OFF)

add_executable(single_pipe single_pipe.cpp)
target_link_libraries(single_pipe PRIVATE phgasnets ${CERES_LIBRARIES} nlohmann_json::nlohmann_json HighFive)

if(PHGASNETS_NUMERICDIFF)
    set_source_files_properties(single_pipe PROPERTIES COMPILE_DEFINITIONS "PHGASNETS_NUMERICDIFF=1")
else()
    set_source_files_properties(single_pipe PROPERTIES COMPILE_DEFINITIONS "PHGASNETS_NUMERICDIFF=0")
endif()
