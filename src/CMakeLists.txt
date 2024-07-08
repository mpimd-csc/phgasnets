# target
add_library(phgasnets derivative.cpp gasconstant.cpp operators.cpp compressor.cpp utils.cpp io.cpp)

target_include_directories(phgasnets PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/phgasnets>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(phgasnets PUBLIC HighFive)
