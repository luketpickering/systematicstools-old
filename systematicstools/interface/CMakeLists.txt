cet_make(LIBRARY_NAME systematicstools_interface
  SOURCE
    EventResponse_product.cc
    ISystProviderTool.cc
    SystMetaData.cc
    SystParamHeader.cc
  LIBRARIES
    ${FHICLCPP}
    cetlib
    cetlib_except
  )

install_headers()
