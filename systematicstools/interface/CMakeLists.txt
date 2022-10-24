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
  ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - deprecated: use cet_make_library(), build_dictonary(), cet_plugin() with explicit source lists and plugin base types

install_headers()
