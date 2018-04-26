#ifndef LARSYST_UTILITY_BUILDPARAMETERSETFROMHEADER_SEEN
#define LARSYST_UTILITY_BUILDPARAMETERSETFROMHEADER_SEEN
#include "larsyst/interface/SystMetaData.hh"

#include "fhiclcpp/ParameterSet.h"

namespace larsyst {
fhicl::ParameterSet build_parameter_set_from_header(SystParamHeader const &sph);
}
#endif
