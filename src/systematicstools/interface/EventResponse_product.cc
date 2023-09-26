#include "systematicstools/interface/EventResponse_product.hh"

#include <cmath>

namespace systtools {

bool FullOfUnity(std::vector<double> const &vec,
                 double tolerance = std::numeric_limits<double>::epsilon()) {
  for (auto &u : vec) {
    if (fabs(u - 1.0) > tolerance) {
      return false;
    }
  }
  return true;
}

void ScrubUnityEventResponses(std::unique_ptr<EventResponse> &er) {
  for (event_unit_response_t &eur : (*er)) {
    ScrubUnityEventResponses(eur);
  }
}

void ScrubUnityEventResponses(event_unit_response_t &eur) {
  for (event_unit_response_t::iterator resp_it = eur.begin();
       resp_it != eur.end();) {
    if (FullOfUnity(resp_it->responses)) {
      resp_it = eur.erase(resp_it);
    } else {
      ++resp_it;
    }
  }
}

} // namespace systtools
