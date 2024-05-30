# Ceres
find_package(Ceres 2.0.0 REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# nlohmann_json
find_package(nlohmann_json 3.10 REQUIRED)

# add all demo subfolders
file(GLOB V_GLOB LIST_DIRECTORIES true "*")
foreach(item ${V_GLOB})
	if(IS_DIRECTORY ${item})
		add_subdirectory(${item})
	endif()
endforeach()
