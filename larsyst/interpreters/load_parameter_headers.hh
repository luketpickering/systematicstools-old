#ifndef LARSYST_INTERPRETERS_LOADPARAMETERHEADERS_SEEN
#define LARSYST_INTERPRETERS_LOADPARAMETERHEADERS_SEEN

#include "larsyst/interface/types.hh"
#include "larsyst/interface/SystMetaData.hh"

#include "fhiclcpp/ParameterSet.h"

#include <string>

namespace larsyst {
SystParamHeader build_header_from_parameter_set(fhicl::ParameterSet const &);

param_header_map_t
load_syst_provider_headers(fhicl::ParameterSet const &paramset, std::string const &key="syst_providers");
} // namespace larsyst

#endif
