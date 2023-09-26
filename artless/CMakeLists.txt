include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/systematicstoolsDependencies.cmake)

####### Interface library
SET(interface_IMPLFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/EventResponse_product.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/ISystProviderTool.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/FHiCLSystParamHeaderConverters.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/SystMetaData.cc
${CMAKE_SOURCE_DIR}/systematicstools/interface/SystParamHeader.cc)
SET(interface_HDRFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/EventResponse_product.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/ISystProviderTool.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/FHiCLSystParamHeaderConverters.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/SystMetaData.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/SystParamHeader.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interface/types.hh)

add_library(systematicstools_interface SHARED ${interface_IMPLFILES})
set_target_properties(systematicstools_interface PROPERTIES 
  PUBLIC_HEADER "${interface_HDRFILES}"
  EXPORT_NAME interface)

target_link_libraries(systematicstools_interface ROOT::ROOT systematicstools_dependencies)

install(TARGETS systematicstools_interface 
    EXPORT systematicstools-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/systematicstools/interface)

####### Util library
SET(utility_IMPLFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/FHiCLSystParamHeaderUtility.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ParameterAndProviderConfigurationUtility.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ResponselessParamUtility.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/md5.cc)

SET(utility_HDRFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/FHiCLSystParamHeaderUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ParameterAndProviderConfigurationUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ResponselessParamUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/md5.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/printers.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/ROOTUtility.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/string_parsers.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/utility/exceptions.hh)

add_library(systematicstools_utility SHARED ${utility_IMPLFILES})
set_target_properties(systematicstools_utility PROPERTIES 
  PUBLIC_HEADER "${utility_HDRFILES}"
  EXPORT_NAME utility)

target_link_libraries(systematicstools_utility systematicstools_interface)

install(TARGETS systematicstools_utility 
    EXPORT systematicstools-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/systematicstools/utility)

####### Interpreter library
SET(interpreters_IMPLFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamHeaderHelper.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamValidationAndErrorResponse.cc)

SET(interpreters_HDRFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/EventSplineCacheHelper.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamHeaderHelper.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/PolyResponse.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/PrecalculatedResponseReader.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamValidationAndErrorResponse.hh)

add_library(systematicstools_interpreters SHARED ${interpreters_IMPLFILES})
set_target_properties(systematicstools_interpreters PROPERTIES 
  PUBLIC_HEADER "${interpreters_HDRFILES}"
  EXPORT_NAME interpreters)

target_link_libraries(systematicstools_interpreters systematicstools_utility)

install(TARGETS systematicstools_interpreters 
    EXPORT systematicstools-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/systematicstools/interpreters)

####### SystProviders library

SET(systproviders_IMPLFILES
${CMAKE_SOURCE_DIR}/systematicstools/systproviders/ExampleISystProvider_tool.cc)

SET(systproviders_HDRFILES
${CMAKE_SOURCE_DIR}/systematicstools/systproviders/ExampleISystProvider_tool.hh)

add_library(systematicstools_systproviders SHARED ${systproviders_IMPLFILES})
set_target_properties(systematicstools_systproviders PROPERTIES 
  PUBLIC_HEADER "${systproviders_HDRFILES}"
  EXPORT_NAME systproviders)

target_link_libraries(systematicstools_systproviders systematicstools_utility)

install(TARGETS systematicstools_systproviders 
    EXPORT systematicstools-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/systematicstools/systproviders)

####### Apps

set(APPS_TO_BUILD
  GenerateSystProviderConfig
  FindISystProvider
  DumpSystToolsParameters)

foreach(APP ${APPS_TO_BUILD})
  add_executable(${APP} 
    ${CMAKE_SOURCE_DIR}/systematicstools/app/${APP}.cc)

  target_link_libraries(${APP}
    systematicstools_systproviders
    systematicstools_utility
    systematicstools_interpreters
    systematicstools_interface)

  INSTALL(TARGETS ${APP} DESTINATION bin)
endforeach()

# ####### fhicl files
# file(GLOB FCL ${CMAKE_SOURCE_DIR}/systematicstools/fcl/*.fcl)
# install(FILES ${FCL} DESTINATION ${CMAKE_INSTALL_PREFIX}/fcl)

# install(FILES
#   ${CMAKE_CURRENT_SOURCE_DIR}/systematicstoolsConfig.cmake DESTINATION
#   ${CMAKE_INSTALL_PREFIX}/cmake)

# install(EXPORT systematicstools-targets
#   FILE systematicstoolsTargets.cmake
#   NAMESPACE systematicstools::
#   DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake
# )

# FILE(WRITE ${CMAKE_INSTALL_PREFIX}/cmake/systematicstoolsVersion.cmake 
#   "SET(systematicstools_VERSION ${systematicstools_VERSION})\n" )

# ####### environment scripts
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.sh.in
# "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" @ONLY)

# install(FILES
#   "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.sh" DESTINATION
#   ${CMAKE_INSTALL_PREFIX})
