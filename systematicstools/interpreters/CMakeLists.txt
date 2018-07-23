cet_make(LIBRARY_NAME systematicstools_interpreters
  LIBRARIES
    ${FHICLCPP}
    cetlib
    cetlib_except
    ${ROOT_BASIC_LIB_LIST}
  )

install_headers()
