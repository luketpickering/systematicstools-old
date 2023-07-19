####### Util library

SET(UTIL_IMPLFILES
  FHiCLSystParamHeaderUtility.cc
  ParameterAndProviderConfigurationUtility.cc
  ResponselessParamUtility.cc
  md5.cc)

SET(UTIL_HDRFILES
  FHiCLSystParamHeaderUtility.hh
  ParameterAndProviderConfigurationUtility.hh
  ResponselessParamUtility.hh
  md5.hh
  printers.hh
  ROOTUtility.hh
  string_parsers.hh
  exceptions.hh)

add_library(systematicstools_utility SHARED ${UTIL_IMPLFILES})
add_library(systtools::utility ALIAS systematicstools_utility)

set_target_properties(systematicstools_utility PROPERTIES 
  PUBLIC_HEADER "${UTIL_HDRFILES}"
  EXPORT_NAME utility )

target_link_libraries(systematicstools_utility PUBLIC systtools::interface ROOT::Core)

install(TARGETS systematicstools_utility
    EXPORT systtools-targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    PUBLIC_HEADER DESTINATION include/systematicstools/utility COMPONENT Development)
