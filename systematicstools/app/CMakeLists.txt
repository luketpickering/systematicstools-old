cet_make_exec(GenerateSystProviderConfig
  LIBRARIES
    systematicstools_utility
    systematicstools_interpreters
    ${FHICLCPP}
    art_Utilities
    cetlib
    cetlib_except
  )

cet_make_exec(CheckSystProviderConfigmd5
  LIBRARIES
    systematicstools_utility
    ${FHICLCPP}
    art_Utilities
    cetlib
    cetlib_except
  )

cet_make_exec(DumpSystToolsParameters
  LIBRARIES
    systematicstools_utility
    systematicstools_interface
    ${FHICLCPP}
    art_Utilities
    cetlib
    cetlib_except
  )


cet_make_exec(FindISystProvider
  LIBRARIES
    ${FHICLCPP}
    art_Utilities
    cetlib
    cetlib_except
  )
