####### SystProviders library

SET(SPV_IMPLFILES
${CMAKE_SOURCE_DIR}/systematicstools/systproviders/ExampleISystProvider_tool.cc)

SET(SPV_HDRFILES
${CMAKE_SOURCE_DIR}/systematicstools/systproviders/ExampleISystProvider_tool.hh)


if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(systematicstools_systproviders STATIC ${SPV_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(systematicstools_systproviders SHARED ${SPV_IMPLFILES})
endif()

add_dependencies(systematicstools_systproviders fhiclcpp-simple)

INSTALL(TARGETS systematicstools_systproviders DESTINATION lib)
INSTALL(FILES ${SPV_HDRFILES} DESTINATION include/systematicstools/systproviders)
