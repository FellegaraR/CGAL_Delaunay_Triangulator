cmake_minimum_required(VERSION 3.5)

# Set the project name
project (CGAL_Delaunay_Triangulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Release")

# find a boost install with the libraries filesystem and system
find_package(VTK REQUIRED)
find_package(CGAL REQUIRED)

set(SOURCES 
	sources/string_management.h
	sources/string_management.cpp
    sources/io.h
	sources/io.cpp
    sources/main_tri.cpp)

# Add an executable
add_executable(dtri ${SOURCES})

# link against the boost libraries
target_link_libraries(dtri CGAL::CGAL ${VTK_LIBRARIES})