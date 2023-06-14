####### Interface library
SET(IFCE_IMPLFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/EventResponse_product.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/ISystProviderTool.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/FHiCLSystParamHeaderConverters.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/SystMetaData.cc
${CMAKE_SOURCE_DIR}/systematicstools/interface/SystParamHeader.cc)
SET(IFCE_HDRFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/EventResponse_product.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/ISystProviderTool.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/FHiCLSystParamHeaderConverters.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/SystMetaData.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/SystParamHeader.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/types.hh)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(systematicstools_interface STATIC ${IFCE_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(systematicstools_interface SHARED ${IFCE_IMPLFILES})
endif()

add_dependencies(systematicstools_interface fhiclcpp-simple)

INSTALL(TARGETS systematicstools_interface DESTINATION lib)
INSTALL(FILES ${IFCE_HDRFILES} DESTINATION include/systematicstools/interface)
