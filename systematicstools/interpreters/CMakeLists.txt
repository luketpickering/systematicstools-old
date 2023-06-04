####### Interpreter library
SET(INTR_IMPLFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamHeaderHelper.cc
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamValidationAndErrorResponse.cc)

SET(INTR_HDRFILES
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/EventSplineCacheHelper.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamHeaderHelper.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/PolyResponse.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/PrecalculatedResponseReader.hh
  ${CMAKE_SOURCE_DIR}/systematicstools/interpreters/ParamValidationAndErrorResponse.hh)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(systematicstools_interpreters STATIC ${INTR_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(systematicstools_interpreters SHARED ${INTR_IMPLFILES})
endif()

add_dependencies(systematicstools_interpreters fhiclcpp-simple)

INSTALL(TARGETS systematicstools_interpreters DESTINATION lib)
INSTALL(FILES ${INTR_HDRFILES} DESTINATION include/systematicstools/interpreters)
