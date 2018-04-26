#ifndef LARSYST_UTILITY_CONFIGURESYSTPROVIDERS_SEEN
#define LARSYST_UTILITY_CONFIGURESYSTPROVIDERS_SEEN

#include "larsyst/interface/ISystProvider_tool.hh"
#include "larsyst/interface/types.hh"

#include "fhiclcpp/ParameterSet.h"

#include <memory>
#include <string>

namespace larsyst {

provider_map_t configure_syst_providers(fhicl::ParameterSet const &paramset, std::string const &key="syst_providers", paramId_t offset = 0);
provider_map_t
load_syst_provider_configuration(fhicl::ParameterSet const &paramset, std::string const &key="syst_providers");

} // namespace larsyst

#endif
