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
  MathCore)

message (STATUS "[ROOT]: root-config --version: ${ROOT_VERSION} ")
message (STATUS "[ROOT]: root-config --cflags : ${ROOT_CXX_FLAGS_SCRUBBED} ")
message (STATUS "[ROOT]: libs in use          : ${ROOT_LIBS} ")

###### Compiler set up
  if(CMAKE_LINK_FLAGS STREQUAL "")
    SET(CMAKE_LINK_FLAGS "-L${ROOT_LIBDIR}")
  else()
    SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -L${ROOT_LIBDIR}")
  endif()
foreach(OBJ ${NEUT_ROOT_LIBS})
  SET(CMAKE_LINK_FLAGS " -l${OBJ}")
endforeach()

if(CMAKE_CXX_FLAGS STREQUAL "")
  SET(CMAKE_CXX_FLAGS " -DNO_ART -std=c++1y ${ROOT_CXX_FLAGS_SCRUBBED}")
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_ART -std=c++1y ${ROOT_CXX_FLAGS_SCRUBBED}")
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_INSTALL_PREFIX}/include)

####### Util library
SET(UTIL_IMPLFILES
  ${CMAKE_SOURCE_DIR}/larsyst/utility/FHiCLSystParamHeaderUtility.cc
  ${CMAKE_SOURCE_DIR}/larsyst/utility/ParameterAndProviderConfigurationUtility.cc
  ${CMAKE_SOURCE_DIR}/larsyst/utility/ResponselessParamUtility.cc
  ${CMAKE_SOURCE_DIR}/larsyst/utility/md5.cc)
SET(UTIL_HDRFILES
  ${CMAKE_SOURCE_DIR}/larsyst/utility/FHiCLSystParamHeaderUtility.hh
  ${CMAKE_SOURCE_DIR}/larsyst/utility/ParameterAndProviderConfigurationUtility.hh
  ${CMAKE_SOURCE_DIR}/larsyst/utility/ResponselessParamUtility.hh
  ${CMAKE_SOURCE_DIR}/larsyst/utility/md5.hh
  ${CMAKE_SOURCE_DIR}/larsyst/utility/printers.hh
  ${CMAKE_SOURCE_DIR}/larsyst/utility/ROOTUtility.hh
  ${CMAKE_SOURCE_DIR}/larsyst/utility/string_parsers.hh
  ${CMAKE_SOURCE_DIR}/larsyst/utility/exceptions.hh)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(larsyst_utility STATIC ${UTIL_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(larsyst_utility SHARED ${UTIL_IMPLFILES})
endif()

add_dependencies(larsyst_utility fhiclcpp-simple)

INSTALL(TARGETS larsyst_utility DESTINATION lib)
INSTALL(FILES ${UTIL_HDRFILES} DESTINATION include/larsyst/utility)

####### Interface library
SET(IFCE_IMPLFILES
  ${CMAKE_SOURCE_DIR}/larsyst/interface/EventResponse_product.cc
  ${CMAKE_SOURCE_DIR}/larsyst/interface/ISystProvider_tool.cc
  ${CMAKE_SOURCE_DIR}/larsyst/interface/SystMetaData.cc
${CMAKE_SOURCE_DIR}/larsyst/interface/SystParamHeader.cc)
SET(IFCE_HDRFILES
  ${CMAKE_SOURCE_DIR}/larsyst/interface/EventResponse_product.hh
  ${CMAKE_SOURCE_DIR}/larsyst/interface/ISystProvider_tool.hh
  ${CMAKE_SOURCE_DIR}/larsyst/interface/SystMetaData.hh
  ${CMAKE_SOURCE_DIR}/larsyst/interface/SystParamHeader.hh
  ${CMAKE_SOURCE_DIR}/larsyst/interface/types.hh)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(larsyst_interface STATIC ${IFCE_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(larsyst_interface SHARED ${IFCE_IMPLFILES})
endif()

add_dependencies(larsyst_interface fhiclcpp-simple)

INSTALL(TARGETS larsyst_interface DESTINATION lib)
INSTALL(FILES ${IFCE_HDRFILES} DESTINATION include/larsyst/interface)

####### Interpreter library
SET(INTR_IMPLFILES
  ${CMAKE_SOURCE_DIR}/larsyst/interpreters/ParamHeaderHelper.cc
  ${CMAKE_SOURCE_DIR}/larsyst/interpreters/ParamValidationAndErrorResponse.cc)

SET(INTR_HDRFILES
  ${CMAKE_SOURCE_DIR}/larsyst/interpreters/EventSplineCacheHelper.hh
  ${CMAKE_SOURCE_DIR}/larsyst/interpreters/ParamHeaderHelper.hh
  ${CMAKE_SOURCE_DIR}/larsyst/interpreters/ParamValidationAndErrorResponse.hh)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(larsyst_interpreters STATIC ${INTR_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(larsyst_interpreters SHARED ${INTR_IMPLFILES})
endif()

add_dependencies(larsyst_interpreters fhiclcpp-simple)

INSTALL(TARGETS larsyst_interpreters DESTINATION lib)
INSTALL(FILES ${INTR_HDRFILES} DESTINATION include/larsyst/interpreters)

####### SystProviders library

SET(SPV_IMPLFILES
${CMAKE_SOURCE_DIR}/larsyst/systproviders/ExampleISystProvider_tool.cc)

SET(SPV_HDRFILES
${CMAKE_SOURCE_DIR}/larsyst/systproviders/ExampleISystProvider_tool.hh)


if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(larsyst_systproviders STATIC ${SPV_IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(larsyst_systproviders SHARED ${SPV_IMPLFILES})
endif()

add_dependencies(larsyst_systproviders fhiclcpp-simple)

INSTALL(TARGETS larsyst_systproviders DESTINATION lib)
INSTALL(FILES ${SPV_HDRFILES} DESTINATION include/larsyst/systproviders)

####### GenerateSystProvider app

add_executable(GenerateSystProviderConfig ${CMAKE_SOURCE_DIR}/larsyst/app/GenerateSystProviderConfig.cc)

target_link_libraries(GenerateSystProviderConfig larsyst_interface)
target_link_libraries(GenerateSystProviderConfig larsyst_interpreters)
target_link_libraries(GenerateSystProviderConfig larsyst_systproviders)
target_link_libraries(GenerateSystProviderConfig larsyst_utility)
add_dependencies(GenerateSystProviderConfig fhiclcpp-simple)

INSTALL(TARGETS GenerateSystProviderConfig DESTINATION bin)

####### fhicl files
file(GLOB FCL ${CMAKE_SOURCE_DIR}/larsyst/fcl/*.fcl)
install(FILES ${FCL} DESTINATION ${CMAKE_INSTALL_PREFIX}/fcl)


####### environment scripts
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.sh.in
"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/larsyst.setup.sh" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/larsyst.setup.sh" DESTINATION
  ${CMAKE_INSTALL_PREFIX})
