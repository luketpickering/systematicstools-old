cet_make(LIBRARY_NAME systematicstools_interpreters
  LIBRARIES
    ${FHICLCPP}
    cetlib
    cetlib_except
    ${ROOT_BASIC_LIB_LIST}
  ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - deprecated: use cet_make_library(), build_dictonary(), cet_plugin() with explicit source lists and plugin base types

install_headers()
