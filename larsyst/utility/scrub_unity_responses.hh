#ifndef LARSYST_UTILITY_SCRIBUNITRESPONSES_SEEN
#define LARSYST_UTILITY_SCRIBUNITRESPONSES_SEEN

#include "larsyst/interface/EventResponse_product.hh"

#include <memory>

namespace larsyst {
inline bool full_of_unity(std::vector<double> const &vec) {
  for (auto &u : vec) {
    if (u != 1) {
      return false;
    }
  }
  return true;
}
inline void scrub_unity_responses(std::unique_ptr<EventResponse> &er) {
  for (auto &eur : er->responses) {
    for (event_unit_response_t::iterator resp_it = eur.begin();
         resp_it != eur.end();) {
      if (full_of_unity(resp_it->second)) {
        std::cout << "[INFO]: Event response to all variations of parameter "
                  << resp_it->first << " was unity. " << std::endl;
        resp_it = eur.erase(resp_it);
      } else {
        ++resp_it;
      }
    }
  }
}
} // namespace larsyst

#endif
