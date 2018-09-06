#include "systematicstools/interface/EventResponse_product.hh"

#include <cmath>

namespace systtools {

void ExtendEventResponse(std::unique_ptr<EventResponse> &e1,
                         std::unique_ptr<EventResponse> &&e2) {

  if (!e2) {
    return;
  }

  if (e1->size() != e2->size()) {
    throw incompatible_number_of_event_units()
        << "[ERROR]: The number of responses from two systematic "
           "providers differs, Provider 1 has "
        << e1->size() << ", and 2 has " << e2->size();
    throw;
  }
  size_t NResponses = e1->size();
  for (size_t eur_it = 0; eur_it < NResponses; ++eur_it) {
    for (ParamResponses &resps : e2->at(eur_it)) {
      if (ContainterHasParam(e1->at(eur_it), resps.pid)) {
        throw systParamId_collision()
            << "[ERROR]: Failed to insert response of parameter ID = "
            << resps.pid << ", it already exists.";
      }
      e1->at(eur_it).push_back(std::move(resps));
    }
  }
}

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
