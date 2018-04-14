# basic source code CMakeLists.txt
add_subdirectory(interface)
add_subdirectory(module)
add_subdirectory(syst_providers)
add_subdirectory(fcl)

install_headers()
install_source()
install_fhicl()
