# include required argparse for demos
find_package(cxxopts 3.0.0 REQUIRED)
include_directories(${CXXOPTS_INCLUDE_DIR})

# add all demo subfolders
file(GLOB V_GLOB LIST_DIRECTORIES true "*")
foreach(item ${V_GLOB})
	if(IS_DIRECTORY ${item})
		add_subdirectory(${item})
	endif()
endforeach()
