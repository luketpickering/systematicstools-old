# ======================================================================
#  systematicstools main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/systematicstools/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================

# use cmake 3.3 or later
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

project(systematicstools)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

#In case we want to use CPM?
#LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
#include(CPM)

################################################################################
#                            Check Dependencies
################################################################################

###### ROOT set up

if (NOT DEFINED ENV{ROOTSYS})
  message (FATAL_ERROR "$ROOTSYS is not defined, please set up ROOT first.")
else()
  message(STATUS "Using ROOT installed at $ENV{ROOTSYS}")
  set(CMAKE_ROOTSYS $ENV{ROOTSYS})
endif()

# Get cflags from ROOT
execute_process (COMMAND root-config
  --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get libdir from ROOT
execute_process (COMMAND root-config
  --libdir OUTPUT_VARIABLE ROOT_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get version from ROOT
execute_process (COMMAND root-config
  --version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get features from ROOT
execute_process (COMMAND root-config
  --features OUTPUT_VARIABLE ROOT_FEATURES OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(ROOT_LIBS)
LIST(APPEND ROOT_LIBS
  Core
  MathCore
  Hist)

message (STATUS "[ROOT]: root-config --version: ${ROOT_VERSION} ")
message (STATUS "[ROOT]: root-config --cflags : ${ROOT_CXX_FLAGS} ")
message (STATUS "[ROOT]: libs in use          : ${ROOT_LIBS} ")

###### Compiler set up
  if(CMAKE_LINK_FLAGS STREQUAL "")
    SET(CMAKE_LINK_FLAGS "-L${ROOT_LIBDIR}")
  else()
    SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -L${ROOT_LIBDIR}")
  endif()

if(CMAKE_CXX_FLAGS STREQUAL "")
  SET(CMAKE_CXX_FLAGS " -DNO_ART -Wall -Wextra -Werror ${ROOT_CXX_FLAGS}")
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_ART -std=c++1y -Wall -Wextra -Werror ${ROOT_CXX_FLAGS}")
endif()

###### fhiclcpp-simple set up

include(ExternalProject)

ExternalProject_Add(fhiclcpp-simple
  PREFIX "${PROJECT_BINARY_DIR}/fhiclcpp-simple"
  GIT_REPOSITORY https://github.com/luketpickering/fhiclcpp-simple.git
  GIT_TAG stable
  UPDATE_DISCONNECTED 0
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DSETUPFILENAME=fhiclcpp.setup.sh
)


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(systematicstools)
