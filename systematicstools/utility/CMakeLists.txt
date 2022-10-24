art_make(LIBRARY_NAME systematicstools_utility
  LIB_LIBRARIES
    systematicstools_interface
    ${ART_FRAMEWORK_CORE}
    ${ART_FRAMEWORK_PRINCIPAL}
    art_Persistency_Common
    art_Persistency_Provenance
    art_Utilities
    canvas
    ${FHICLCPP}
    cetlib
    cetlib_except
    ${CLHEP}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${ROOT_BASIC_LIB_LIST}
) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - deprecated: use art_make_library(), art_dictonary(), and cet_build_plugin() with explicit source lists and plugin base types

install_headers()
