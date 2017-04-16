cmake_minimum_required(VERSION 2.8)

# Name of the project
project(raytrace)

# Set the CMake module folder (for custom modules)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}\cmake)

# Dependencies
find_package(Eigen3 REQUIRED)

# Use glob to get the list of all source files.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# We don't really need to include header and resource files to build, but it's
# nice to have them show up in IDEs.
file(GLOB_RECURSE HEADERS "src/*.hpp")
include_directories(${EIGEN3_INCLUDE_DIR})

# Set the executable.
add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})

# OS specific options and libraries
if(WIN32)
  # c++0x is enabled by default.
  # -Wall produces way too many warnings.
  # -pedantic is not supported.
  # Disable warning 4996.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
else()
  # Enable all pedantic warnings.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")
endif()