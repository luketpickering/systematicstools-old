#ifndef LARSYST_UTILITY_BUILDPARAMETERSETFROMHEADER_SEEN
#define LARSYST_UTILITY_BUILDPARAMETERSETFROMHEADER_SEEN

namespace fhicl {
class ParameterSet;
}

namespace larsyst {
struct SystParamHeader;
fhicl::ParameterSet build_parameter_set_from_header(SystParamHeader const &sph);
} // namespace larsyst
#endif
