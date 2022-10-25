cet_make_library(LIBRARY_NAME systematicstools_interpreters
  SOURCE
    ParamHeaderHelper.cc
    ParamValidationAndErrorResponse.cc
  LIBRARIES
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
    ROOT::Hist
    ROOT::Core
  )

install_headers()
