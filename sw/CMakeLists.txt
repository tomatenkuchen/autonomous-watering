##################################################################################################
# cmake boilerplate
cmake_minimum_required(VERSION 4.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "build type: Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "project build type" FORCE)
endif()

###################################################################################################
# dependencies

list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators")

find_package(etl REQUIRED)

###################################################################################################
# set up esp-idf environment
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
 
###################################################################################################
# project definition

include(${CMAKE_BINARY_DIR}/version.cmake)

project(${CONAN_PROJECT_NAME}
  DESCRIPTION ${CONAN_PROJECT_DESCRIPTION}
  VERSION ${CONAN_PROJECT_VERSION_MAJOR}.${CONAN_PROJECT_VERSION_MINOR}.${CONAN_PROJECT_VERSION_PATCH}
  LANGUAGES C CXX
)

configure_file(
   ${CMAKE_SOURCE_DIR}/cmake/version.hpp.in
   ${CMAKE_BINARY_DIR}/version.hpp
)
