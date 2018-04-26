#ifndef LARSYST_UTILITY_GENERATEPROVIDERPARAMETERSET_SEEN
#define LARSYST_UTILITY_GENERATEPROVIDERPARAMETERSET_SEEN

#include "larsyst/interface/types.hh"

#include "fhiclcpp/ParameterSet.h"

namespace larsyst {
fhicl::ParameterSet
generate_provider_parameter_set(larsyst::provider_map_t::mapped_type const &);
fhicl::ParameterSet generate_provider_parameter_set(
    std::pair<std::pair<std::string, std::string>,
              std::vector<SystParamHeader>> const &);
} // namespace larsyst
#endif
