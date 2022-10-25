cet_make_library(LIBRARY_NAME systematicstools_utility
  SOURCE
    CovMatThrower.cc
    FHiCLSystParamHeaderUtility.cc
    md5.cc
    ParameterAndProviderConfigurationUtility.cc
    ResponselessParamUtility.cc
  LIBRARIES
    systematicstools::interface
    art::Framework_Core
    art::Framework_Principal
    art::Persistency_Common
    art::Persistency_Provenance
    art::Utilities
    canvas::canvas
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
    CLHEP::Random
    Boost::filesystem
    ROOT::Matrix
    ROOT::Core
)

install_headers()
