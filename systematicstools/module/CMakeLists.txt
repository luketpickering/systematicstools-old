art_make(LIBRARY_NAME systematicstools
  EXCLUDE
    SystToolsResponseTreeMaker_module.cc
  LIB_LIBRARIES
    ${ART_FRAMEWORK_CORE}
    ${ART_FRAMEWORK_PRINCIPAL}
    art_Utilities
    canvas
    ${FHICLCPP}
    cetlib
    cetlib_except
    ${MF_MESSAGELOGGER}
    ${MF_UTILITIES}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
  MODULE_LIBRARIES
    systematicstools_utility
    systematicstools_interface
    ${ART_PERSISTENCY_COMMON}
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
    ${ART_ROOT_IO_TFILESERVICE_SERVICE}
    ${ROOT_BASIC_LIB_LIST}
) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - deprecated: use art_make_library(), art_dictonary(), and cet_build_plugin() with explicit source lists and plugin base types
