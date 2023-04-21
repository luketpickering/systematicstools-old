get_filename_component(nusystematicsDependencies_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include(${nusystematicsDependencies_CMAKE_DIR}/CPM.cmake)
CPMFindPackage(
    NAME CMakeModules
    GIT_TAG stable
    GITHUB_REPOSITORY NuHepMC/CMakeModules
    DOWNLOAD_ONLY
)
include(${CMakeModules_SOURCE_DIR}/NuHepMCModules.cmake)

include(CMessage)

CPMFindPackage(
    NAME fhiclcpp
    GIT_TAG stable
    GITHUB_REPOSITORY luketpickering/fhiclcpp-simple
)

include(ROOT)

###### Compiler set up
add_library(systematicstools_dependencies INTERFACE)
target_link_libraries(systematicstools_dependencies INTERFACE 
  fhiclcpp::includes)
target_compile_definitions(systematicstools_dependencies INTERFACE 
  NO_ART)
target_include_directories(systematicstools_dependencies INTERFACE 
  ${nusystematicsDependencies_CMAKE_DIR}/../../../../)