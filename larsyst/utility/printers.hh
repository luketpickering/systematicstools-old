#ifndef LARSYST_UTILITY_PRINTERS_SEEN
#define LARSYST_UTILITY_PRINTERS_SEEN

#include "larsyst/interface/SystMetaData.hh"
#include "build_parameter_set_from_header.hh"

#include <string>

namespace larsyst {
inline std::string to_str(SystParamHeader const &sph, bool indent = true) {
  fhicl::ParameterSet ps = build_parameter_set_from_header(sph);
  return indent ? ps.to_indented_string() : ps.to_string();
}
} // namespace larsyst

#endif
