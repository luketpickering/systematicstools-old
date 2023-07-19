####### Interpreter library
SET(INTR_IMPLFILES
  ParamHeaderHelper.cc
  ParamValidationAndErrorResponse.cc)

SET(INTR_HDRFILES
  EventSplineCacheHelper.hh
  ParamHeaderHelper.hh
  PolyResponse.hh
  PrecalculatedResponseReader.hh
  ParamValidationAndErrorResponse.hh)

add_library(systematicstools_interpreters SHARED ${INTR_IMPLFILES})
add_library(systtools::interpreters ALIAS systematicstools_interpreters)

set_target_properties(systematicstools_interpreters PROPERTIES 
  PUBLIC_HEADER "${INTR_HDRFILES}"
  EXPORT_NAME interpreters )

target_link_libraries(systematicstools_interpreters PUBLIC systtools::utility)

install(TARGETS systematicstools_interpreters
    EXPORT systtools-targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    PUBLIC_HEADER DESTINATION include/systematicstools/interpreters COMPONENT Development)
