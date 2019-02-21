
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set project name
project(Tbl)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Create library as project name
add_library(${PROJECT_NAME} INTERFACE)
add_custom_target(Header SOURCES "Include/Tbl.hpp")

# Check if TableTest is included via add_subdirectory.
get_directory_property(tabletop_is_subproject PARENT_DIRECTORY)

# Only build the test suite if this is not a subproject
if(NOT tabletop_is_subproject)
	add_subdirectory("Tests/UnitTests")
	set_property(TARGET UnitTests PROPERTY FOLDER "Tests")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT UnitTests)
endif()
