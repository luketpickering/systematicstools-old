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
cmake_minimum_required (VERSION 3.20 FATAL_ERROR)
# cmake_policy(VERSION 3.18) ### MIGRATE-ACTION-COMPATIBILITY (migrate-3.20.00) - Uncomment to preserve compatibility with older CMake versions

find_package(cetmodules REQUIRED)
project(systematicstools VERSION 01.01.00 LANGUAGES CXX)

# cetbuildtools contains our cmake modules

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules $ENV{ART_DIR}/Modules) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - prefer find_package() to find external CMake modules

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic )

cet_report_compiler_flags() ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - add args: REPORT_THRESHOLD VERBOSE

# these are minimum required versions, not the actual product versions
find_package( canvas REQUIRED )
find_package( canvas_root_io REQUIRED )
find_package( art REQUIRED )
find_package(art_root_io REQUIRED)
find_package(ROOT REQUIRED)

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(systematicstools)

SET(SYSTTOOLS_LIBS
  systematicstools_interface
  systematicstools_interpreters
  systematicstools_utility)


# tests
add_subdirectory(test)

# ups - table and config files

# packaging utility

cet_cmake_config()
