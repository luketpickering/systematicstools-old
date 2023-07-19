####### SystProviders library

SET(SPV_IMPLFILES
  ExampleISystProvider_tool.cc)

SET(SPV_HDRFILES
  ExampleISystProvider_tool.hh)

add_library(systematicstools_systproviders SHARED ${SPV_IMPLFILES})
add_library(systtools::systproviders ALIAS systematicstools_systproviders)

set_target_properties(systematicstools_systproviders PROPERTIES 
  PUBLIC_HEADER "${SPV_HDRFILES}"
  EXPORT_NAME systproviders )

target_link_libraries(systematicstools_systproviders PUBLIC systtools::interface)

install(TARGETS systematicstools_systproviders
    EXPORT systtools-targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    PUBLIC_HEADER DESTINATION include/systematicstools/systproviders COMPONENT Development)
