art_make(LIBRARY_NAME larsyst_utility
  LIB_LIBRARIES
    larsyst_interpreters
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
)

install_headers()
