# target
add_library(phgasnets derivative.cpp gasconstant.cpp operators.cpp pipe.cpp compressor.cpp utils.cpp network.cpp io.cpp steady.cpp transient.cpp)

target_include_directories(phgasnets PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/phgasnets>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(phgasnets PUBLIC HighFive)
