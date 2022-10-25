cet_make_library(LIBRARY_NAME systematicstools_interface
  SOURCE
    EventResponse_product.cc
    FHiCLSystParamHeaderConverters.cc
    ISystProviderTool.cc
    SystMetaData.cc
    SystParamHeader.cc
  LIBRARIES PUBLIC
    art::Framework_Principal
   PRIVATE
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
  )

install_headers()
