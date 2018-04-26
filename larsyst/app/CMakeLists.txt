cet_make_exec(GenerateSystProviderConfig
  LIBRARIES
    larsyst_utility
    larsyst_interpreters
    ${FHICLCPP}
    cetlib
    cetlib_except
  )

cet_make_exec(CheckSystProviderConfigmd5
  LIBRARIES
    larsyst_utility
    ${FHICLCPP}
    cetlib
    cetlib_except
  )
