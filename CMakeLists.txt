# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name
project(GLMandelbrot)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

file(READ ".boglfw_path" BOGLFW_DIST_DIR)
string(STRIP ${BOGLFW_DIST_DIR} BOGLFW_DIST_DIR)

# The following folder will be included
include_directories("${BOGLFW_DIST_DIR}/include")
link_directories("${BOGLFW_DIST_DIR}/lib")
if (WIN32)
	link_directories("C:/mingw64/lib")
	include_directories("C:/mingw64/include")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__WIN32__")
else()
	link_directories("/usr/lib")
	include_directories("/usr/include")
endif()

# add sources
file(GLOB_RECURSE sources	src/*.h)
file(GLOB_RECURSE sources 	src/*.cpp)

add_executable(GLMandelbrot ${sources})

#add libraries
target_link_libraries(GLMandelbrot PUBLIC boglfw SDL2 GLEW png)
if (WIN32)
	target_link_libraries(GLMandelbrot PUBLIC glfw3 opengl32)
else()
	target_link_libraries(GLMandelbrot PUBLIC glfw pthread GL)
endif()

# add explicit dependency on boglfw library to force the target to be re-linked when the library is changed.
# for some reason CMake doesn't do this automatically even though we do depend on the library.
set_property(TARGET GLMandelbrot PROPERTY LINK_DEPENDS "${BOGLFW_DIST_DIR}/lib/libboglfw.a")

set_property(TARGET GLMandelbrot PROPERTY CXX_STANDARD 14)
target_compile_options(GLMandelbrot PUBLIC -Wall -std=c++14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
