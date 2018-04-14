art_make(LIBRARY_NAME larsyst
  LIB_LIBRARIES
    larsyst_interface
    larsyst_providers
    ${ART_FRAMEWORK_CORE}
    ${ART_FRAMEWORK_PRINCIPAL}
    art_Utilities
    canvas
    ${FHICLCPP}
    ${MF_MESSAGELOGGER}
    ${MF_UTILITIES}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
  MODULE_LIBRARIES
    larsyst_interface
)

install_headers()
install_fhicl()
install_source()
