add_subdirectory(interface)
add_subdirectory(interpreters)
add_subdirectory(systproviders)
add_subdirectory(utility)

add_library(systematicstools_all INTERFACE)
target_link_libraries(systematicstools_all INTERFACE 
  systtools::interpreters systtools::systproviders)

set_target_properties(systematicstools_all PROPERTIES EXPORT_NAME all)

add_library(systtools::all ALIAS systematicstools_all)

install(TARGETS 
          systematicstools_all
    EXPORT systtools-targets)