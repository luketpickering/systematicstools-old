cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

set(CMAKE_CXX_STANDARD 17)

project(systematicstools VERSION 23.06 LANGUAGES CXX)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE DebWithRelInfo)
endif()

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
include(CPM)

find_package(ROOT 6.10 REQUIRED)

CPMAddPackage(
    NAME fhiclcpp
    GIT_TAG master
    GITHUB_REPOSITORY luketpickering/fhiclcpp-simple
)

###### Compiler set up
add_library(systematicstools_dependencies INTERFACE)
target_link_libraries(systematicstools_dependencies INTERFACE 
  fhiclcpp::fhiclcpp)
target_include_directories(systematicstools_dependencies INTERFACE 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include> )
set_target_properties(systematicstools_dependencies PROPERTIES EXPORT_NAME commondeps)
add_library(systtools::commondeps ALIAS systematicstools_dependencies)

install(TARGETS 
          systematicstools_dependencies
    EXPORT systtools-targets)

add_subdirectory(src/systematicstools)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/systematicstoolsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/Templates/systematicstoolsConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/systematicstoolsConfig.cmake"
  INSTALL_DESTINATION cmake
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(EXPORT systtools-targets
        NAMESPACE systtools::
        DESTINATION lib/cmake/systematicstools)
install(FILES "${PROJECT_BINARY_DIR}/systematicstoolsConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/systematicstoolsConfig.cmake"
        DESTINATION lib/cmake/systematicstools)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/Templates/setup.systematicstools.sh.in 
  ${PROJECT_BINARY_DIR}/setup.systematicstools.sh @ONLY)
install(PROGRAMS ${PROJECT_BINARY_DIR}/setup.systematicstools.sh DESTINATION bin)

install(DIRECTORY fcl DESTINATION fcl)
