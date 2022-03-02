if(NOT COMMAND cmessage)
  if(NOT WIN32)
    string(ASCII 27 Esc)
    set(CM_ColourReset "${Esc}[m")
    set(CM_ColourBold "${Esc}[1m")
    set(CM_Red "${Esc}[31m")
    set(CM_Green "${Esc}[32m")
    set(CM_Yellow "${Esc}[33m")
    set(CM_Blue "${Esc}[34m")
    set(CM_Magenta "${Esc}[35m")
    set(CM_Cyan "${Esc}[36m")
    set(CM_White "${Esc}[37m")
    set(CM_BoldRed "${Esc}[1;31m")
    set(CM_BoldGreen "${Esc}[1;32m")
    set(CM_BoldYellow "${Esc}[1;33m")
    set(CM_BoldBlue "${Esc}[1;34m")
    set(CM_BoldMagenta "${Esc}[1;35m")
    set(CM_BoldCyan "${Esc}[1;36m")
    set(CM_BoldWhite "${Esc}[1;37m")
  endif()

  message(STATUS "Setting up colored messages...")

  function(cmessage)
    list(GET ARGV 0 MessageType)
    if(MessageType STREQUAL FATAL_ERROR OR MessageType STREQUAL SEND_ERROR)
      list(REMOVE_AT ARGV 0)
      message(${MessageType} "${CM_BoldRed}${ARGV}${CM_ColourReset}")
    elseif(MessageType STREQUAL WARNING)
      list(REMOVE_AT ARGV 0)
      message(${MessageType} "${CM_BoldYellow}${ARGV}${CM_ColourReset}")
    elseif(MessageType STREQUAL AUTHOR_WARNING)
      list(REMOVE_AT ARGV 0)
      message(${MessageType} "${CM_BoldCyan}${ARGV}${CM_ColourReset}")
    elseif(MessageType STREQUAL STATUS)
      list(REMOVE_AT ARGV 0)
      message(${MessageType} "${CM_Green}[INFO]:${CM_ColourReset} ${ARGV}")
    elseif(MessageType STREQUAL CACHE)        
      list(REMOVE_AT ARGV 0)
      message(-- "${CM_Blue}[CACHE]:${CM_ColourReset} ${ARGV}")
    elseif(MessageType STREQUAL DEBUG)
      list(REMOVE_AT ARGV 0)
      if(BUILD_DEBUG_MSGS)
        message("${CM_Magenta}[DEBUG]:${CM_ColourReset} ${ARGV}")
      endif()
    else()
      message(${MessageType} "${CM_Green}[INFO]:${CM_ColourReset} ${ARGV}")
    endif()
  endfunction()
endif()

###### FHICL set up
find_package(fhiclcpp REQUIRED)

###### ROOT set up
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

STRING(STRIP ROOT_CXX_FLAGS ${ROOT_CXX_FLAGS})
STRING(REPLACE " " ";" ROOT_CXX_FLAGS ${ROOT_CXX_FLAGS})

if("${CMAKE_PROJECT_NAME} " STREQUAL "CAFAna ")
  list (FIND ROOT_CXX_FLAGS "-std=c++11" CPP11_INDEX)
  list (FIND ROOT_CXX_FLAGS "-std=c++14" CPP14_INDEX)
  list (FIND ROOT_CXX_FLAGS "-std=c++17" CPP17_INDEX)
  list (FIND ROOT_CXX_FLAGS "-std=c++20" CPP20_INDEX)
  if (${CPP17_INDEX} GREATER -1)
    SET(CMAKE_CXX_STANDARD 17)
  elseif (${CPP20_INDEX} GREATER -1)
    SET(CMAKE_CXX_STANDARD 20)
  else()
    SET(CMAKE_CXX_STANDARD 14)
  endif()
endif()
list(FILTER ROOT_CXX_FLAGS EXCLUDE REGEX "-std=.*")

execute_process (COMMAND root-config
  --version OUTPUT_VARIABLE ROOT_CONFIG_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

#We should let CMake set this
list(REMOVE_ITEM ROOT_CXX_FLAGS "-fPIC")

if(NOT TARGET ROOT::ROOT)
  add_library(ROOT::ROOT INTERFACE IMPORTED)

  LIST(APPEND ROOT_LIB_NAMES 
    Core
    RIO
    Net
    Hist
    Graf
    Graf3d
    Gpad
    Tree
    Rint
    Postscript
    Matrix
    Physics
    MathCore
    Thread
    TMVA)

  foreach(LN ${ROOT_LIB_NAMES})
    if(NOT EXISTS ${ROOT_LIBRARY_DIR}/lib${LN}.so)
      cmessage(FATAL_ERROR "Cannot find ROOT library: ${LN} in expected directory: ${ROOT_LIBRARY_DIR}")
    endif()
  endforeach()

  set_target_properties(ROOT::ROOT PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${ROOT_INCLUDE_DIRS}"
      INTERFACE_COMPILE_OPTIONS "${ROOT_CXX_FLAGS}"
      INTERFACE_LINK_DIRECTORIES "${ROOT_LIBRARY_DIR}"
      INTERFACE_LINK_LIBRARIES "${ROOT_LIB_NAMES}"
  )
  cmessage(STATUS "Built ROOT::ROOT Imported target")
  cmessage(STATUS "        ROOT_INCLUDE_DIRS: ${ROOT_INCLUDE_DIRS}: ")
  cmessage(STATUS "        ROOT_CXX_FLAGS: ${ROOT_CXX_FLAGS}")
  cmessage(STATUS "        ROOT_LIB_DIR: ${ROOT_LIBRARY_DIR}")
  cmessage(STATUS "        ROOT_LIBRARIES: ${ROOT_LIB_NAMES}")
endif()

###### Compiler set up
add_compile_definitions(NO_ART)
include_directories(${CMAKE_SOURCE_DIR})

if(NOT "${CMAKE_PROJECT_NAME} " STREQUAL "systematicstools ")
  get_filename_component(systematicstools_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
  include(${systematicstools_CMAKE_DIR}/systematicstoolsTargets.cmake)
  include(${systematicstools_CMAKE_DIR}/systematicstoolsVersion.cmake)

  find_path(systematicstools_INCLUDE_DIR
    NAMES systematicstools/interface/types.hh
    PATHS ${systematicstools_CMAKE_DIR}/../include
  )

  cmessage(STATUS "systematicstools_INCLUDE_DIR: ${systematicstools_INCLUDE_DIR}")
  cmessage(STATUS "systematicstools_VERSION: ${systematicstools_VERSION}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(systematicstools
      REQUIRED_VARS 
        systematicstools_INCLUDE_DIR
      VERSION_VAR 
        systematicstools_VERSION
  )
  if(NOT TARGET systematicstools::all)
      add_library(systematicstools::all INTERFACE IMPORTED)
      set_target_properties(systematicstools::all PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES ${systematicstools_INCLUDE_DIR}
          INTERFACE_LINK_LIBRARIES 
            "systematicstools::systproviders;systematicstools::interpreters"
      )
  endif()
endif()