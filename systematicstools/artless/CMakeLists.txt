###### FHICL set up
include(ExternalProject)

ExternalProject_Add(fhiclcpp-simple
  PREFIX "${PROJECT_BINARY_DIR}/fhiclcpp-simple"
  GIT_REPOSITORY https://github.com/luketpickering/fhiclcpp-simple.git
  GIT_TAG stable
  UPDATE_DISCONNECTED 1
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DSETUPFILENAME=fhiclcpp.setup.sh)

###### ROOT set up

if (NOT DEFINED ENV{ROOTSYS})
  message (FATAL_ERROR "$ROOTSYS is not defined, please set up ROOT first.")
else()
  message(STATUS "Using ROOT installed at $ENV{ROOTSYS}")
  set(CMAKE_ROOTSYS $ENV{ROOTSYS})
endif()

# Get cflags from ROOT
execute_process (COMMAND root-config
  --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS_RAW OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" ROOT_CXX_FLAGS_PREPARSE "${ROOT_CXX_FLAGS_RAW}")
set(ROOT_CXX_FLAGS_SCRUBBED "")
foreach(FLG ${ROOT_CXX_FLAGS_PREPARSE})
  string(REGEX MATCH "-std=" ISSTDFLAG ${FLG})
  if(NOT ISSTDFLAG) #ignore -std= flags from ROOT config
    SET(ROOT_CXX_FLAGS_SCRUBBED "${ROOT_CXX_FLAGS_SCRUBBED} ${FLG}")
  endif()
endforeach()
# Get libdir from ROOT
execute_process (COMMAND root-config
  --libdir OUTPUT_VARIABLE ROOT_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get version from ROOT
execute_process (COMMAND root-config
  --version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get features from ROOT
execute_process (COMMAND root-config
  --features OUTPUT_VARIABLE ROOT_FEATURES OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(ROOT_LIBS)
LIST(APPEND ROOT_LIBS
  Core
  MathCore
  Hist)

message (STATUS "[ROOT]: root-config --version: ${ROOT_VERSION} ")
message (STATUS "[ROOT]: root-config --cflags : ${ROOT_CXX_FLAGS_SCRUBBED} ")
message (STATUS "[ROOT]: libs in use          : ${ROOT_LIBS} ")

###### Compiler set up
  if(CMAKE_LINK_FLAGS STREQUAL "")
    SET(CMAKE_LINK_FLAGS "-L${ROOT_LIBDIR}")
  else()
    SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -L${ROOT_LIBDIR}")
  endif()

if(CMAKE_CXX_FLAGS STREQUAL "")
  SET(CMAKE_CXX_FLAGS " -DNO_ART -std=c++1y ${ROOT_CXX_FLAGS_SCRUBBED}")
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_ART -std=c++1y ${ROOT_CXX_FLAGS_SCRUBBED}")
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_INSTALL_PREFIX}/include)

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
INSTALL(FILES ${UTIL_HDRFILES} DESTINATION include/systematicstools/utility)

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

####### GenerateSystProvider app

add_executable(GenerateSystProviderConfig ${CMAKE_SOURCE_DIR}/systematicstools/app/GenerateSystProviderConfig.cc)
set_target_properties(GenerateSystProviderConfig PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

target_link_libraries(GenerateSystProviderConfig systematicstools_systproviders)
target_link_libraries(GenerateSystProviderConfig systematicstools_interface)
target_link_libraries(GenerateSystProviderConfig systematicstools_interpreters)
target_link_libraries(GenerateSystProviderConfig systematicstools_utility)
target_link_libraries(GenerateSystProviderConfig ${ROOT_LIBS})
add_dependencies(GenerateSystProviderConfig fhiclcpp-simple)

INSTALL(TARGETS GenerateSystProviderConfig DESTINATION bin)

####### FindISystProvider app

add_executable(FindISystProvider ${CMAKE_SOURCE_DIR}/systematicstools/app/FindISystProvider.cc)
set_target_properties(FindISystProvider PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

target_link_libraries(FindISystProvider systematicstools_systproviders)
target_link_libraries(FindISystProvider systematicstools_interface)
target_link_libraries(FindISystProvider systematicstools_interpreters)
target_link_libraries(FindISystProvider systematicstools_utility)
target_link_libraries(FindISystProvider ${ROOT_LIBS})
add_dependencies(FindISystProvider fhiclcpp-simple)

INSTALL(TARGETS FindISystProvider DESTINATION bin)

####### fhicl files
file(GLOB FCL ${CMAKE_SOURCE_DIR}/systematicstools/fcl/*.fcl)
install(FILES ${FCL} DESTINATION ${CMAKE_INSTALL_PREFIX}/fcl)


####### environment scripts
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.sh.in
"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/systematicstools.setup.sh" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/systematicstools.setup.sh" DESTINATION
  ${CMAKE_INSTALL_PREFIX})
