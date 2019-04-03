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
cmake_minimum_required (VERSION 3.3)

project(systematicstools)

if(NOT DEFINED USEART) #by default build in ART mode
  SET(USEART TRUE)
endif()

if(USEART) # This is the standard, CET-empowered build for building the art module and associated tools

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules
                      $ENV{CETBUILDTOOLS_DIR}/Modules
                      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic )

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( art )
find_ups_product(art_root_io)
find_ups_product( cetbuildtools )

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(systematicstools)

SET(SYSTTOOLS_LIBS
  systematicstools_interface
  systematicstools_interpreters
  systematicstools_utility PARENT_SCOPE)


# tests
add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)

else() #

  #Changes default install path to be a subdirectory of the build dir.
  #Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
  if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
    "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
  elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
  endif()
  message(STATUS "Installing to: ${CMAKE_INSTALL_PREFIX}")

  add_subdirectory(systematicstools/artless)

endif()
