cet_make_exec(GenerateSystProviderConfig
  LIBRARIES
    systematicstools_utility
    systematicstools_interpreters
    ${FHICLCPP}
    cetlib
    cetlib_except
  )

cet_make_exec(CheckSystProviderConfigmd5
  LIBRARIES
    systematicstools_utility
    ${FHICLCPP}
    cetlib
    cetlib_except
  )

cet_make_exec(FindISystProvider
  LIBRARIES
    ${FHICLCPP}
    cetlib
    cetlib_except
  )
