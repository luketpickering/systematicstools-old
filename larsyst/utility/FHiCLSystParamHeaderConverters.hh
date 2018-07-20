#ifndef LARSYST_UTILITY_BUILDPARAMETERSETFROMHEADER_SEEN
#define LARSYST_UTILITY_BUILDPARAMETERSETFROMHEADER_SEEN

namespace fhicl {
class ParameterSet;
}
namespace larsyst {
struct SystParamHeader;
}

namespace larsyst {
///\brief Deserializes a SystParamHeader instance from a passed FHiCL parameter
/// set.
///
/// If building with ART support, then this translation uses the fhicl::Table<>
/// features to provide auto-validation.
SystParamHeader FHiCLToSystParamHeader(fhicl::ParameterSet const &paramset);

///\brief Serializes a SyhstParamHeader instance to a FHiCL table.
fhicl::ParameterSet SystParamHeaderToFHiCL(SystParamHeader const &sph);
} // namespace larsyst
#endif
