SET(SYSTTOOLS_LIBS 
  systematicstools_utility
  systematicstools_interface)

SET(SYSTTOOLS_APPS
  GenerateSystProviderConfig
  CheckSystProviderConfigmd5
  DumpSystToolsParameters
  FindISystProvider)

foreach(APPNAME ${SYSTTOOLS_APPS})
  cet_make_exec(${APPNAME}
  SOURCE ${APPNAME}.cc
  LIBRARIES
    ${SYSTTOOLS_LIBS}
    ${FHICLCPP}
    art_Utilities
    cetlib
    cetlib_except
  )
endforeach()
