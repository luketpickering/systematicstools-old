
SET(SYSTTOOLS_APPS
  GenerateSystProviderConfig
  CheckSystProviderConfigmd5
  DumpSystToolsParameters
  FindISystProvider)

foreach(APPNAME ${SYSTTOOLS_APPS})
  cet_make_exec(NAME ${APPNAME}
  SOURCE ${APPNAME}.cc
  LIBRARIES
    systematicstools::utility
    systematicstools::interface
    fhiclcpp::fhiclcpp
    art::Utilities
    cetlib::cetlib
    cetlib_except::cetlib_except
  )
endforeach()
