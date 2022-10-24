simple_plugin(ExampleISystProvider tool
  systematicstools_interface
  systematicstools_utility
  ${ART_FRAMEWORK_CORE}
  ${ART_FRAMEWORK_PRINCIPAL}
  ${ART_FRAMEWORK_SERVICES_REGISTRY}
  ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
  ${CLHEP}
 ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - deprecated: use cet_build_plugin() with explicit source lists and plugin base types

# simple_plugin(CorrelatedMultisimProvider tool
#   systematicstools_utility
#   ${ART_FRAMEWORK_CORE}
#   ${ART_FRAMEWORK_PRINCIPAL}
#   ${ART_FRAMEWORK_SERVICES_REGISTRY}
#   ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
#   ${CLHEP}
#  )
