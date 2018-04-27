#ifndef LARSYST_UTILITY_PRINTERS_SEEN
#define LARSYST_UTILITY_PRINTERS_SEEN

#include "larsyst/interface/EventResponse_product.hh"
#include "larsyst/interface/SystMetaData.hh"

#include "build_parameter_set_from_header.hh"

#include <string>

namespace larsyst {
inline std::string to_str(SystParamHeader const &sph, bool indent = true) {
  fhicl::ParameterSet ps = build_parameter_set_from_header(sph);
  return indent ? ps.to_indented_string() : ps.to_string();
}
inline std::string to_str(EventResponse const &er) {
  std::stringstream ss("");
  ss << "Event response contains " << er.responses.size() << std::endl;
  for (size_t eur_it = 0; eur_it < er.responses.size(); ++eur_it) {
    auto &eur = er.responses[eur_it];
    ss << "\t\tFound " << eur.size() << " responses to event unit " << eur_it
       << ":" << std::endl;
    for (auto &pr : eur) {
      ss << "\t\t\tParam " << pr.first << ": {" << std::flush;
      for (auto &r : pr.second) {
        ss << r << ", " << std::flush;
      }
      ss << "}" << std::endl;
    }
  }
  return ss.str();
}
} // namespace larsyst

#endif
