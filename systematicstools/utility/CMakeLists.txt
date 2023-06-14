####### Util library

SET(UTIL_IMPLFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/FHiCLSystParamHeaderUtility.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ParameterAndProviderConfigurationUtility.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ResponselessParamUtility.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/md5.cc)
SET(UTIL_HDRFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/FHiCLSystParamHeaderUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ParameterAndProviderConfigurationUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ResponselessParamUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/md5.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/printers.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ROOTUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/string_parsers.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/exceptions.hh)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(systematicstools_utility STATIC ${UTIL_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(systematicstools_utility SHARED ${UTIL_IMPLFILES})
endif()

add_dependencies(systematicstools_utility fhiclcpp-simple)

INSTALL(TARGETS systematicstools_utility DESTINATION lib)
INSTALL(FILES ${UTIL_HDRFILES} DESTINATION include/utility)
