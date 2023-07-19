####### Interface library
SET(IFCE_IMPLFILES
  EventResponse_product.cc
  ISystProviderTool.cc
  FHiCLSystParamHeaderConverters.cc
  SystMetaData.cc
  SystParamHeader.cc)

SET(IFCE_HDRFILES
  EventResponse_product.hh
  ISystProviderTool.hh
  FHiCLSystParamHeaderConverters.hh
  SystMetaData.hh
  SystParamHeader.hh
  types.hh)


add_library(systematicstools_interface SHARED ${IFCE_IMPLFILES})
add_library(systtools::interface ALIAS systematicstools_interface)

set_target_properties(systematicstools_interface PROPERTIES 
  PUBLIC_HEADER "${IFCE_HDRFILES}"
  EXPORT_NAME interface )

target_link_libraries(systematicstools_interface PUBLIC systtools::commondeps)

install(TARGETS systematicstools_interface
    EXPORT systtools-targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    PUBLIC_HEADER DESTINATION include/systematicstools/interface COMPONENT Development)
