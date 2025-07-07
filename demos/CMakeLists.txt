# include required argparse for demos
find_package(argparse 2.9.0 REQUIRED)
include_directories(${ARGPARSE_INCLUDE_DIR})

# add all demo subfolders
file(GLOB V_GLOB LIST_DIRECTORIES true "*")
foreach(item ${V_GLOB})
	if(IS_DIRECTORY ${item})
		add_subdirectory(${item})
	endif()
endforeach()
