#include "append_event_response.hh"

#include <iostream>

NEW_LARSYST_EXCEPT(incompatible_number_of_event_units);

namespace larsyst {
void append_event_response(std::unique_ptr<larsyst::EventResponse> &e1,
                           std::unique_ptr<larsyst::EventResponse> &&e2) {

  if (!e2) {
    return;
  }

  if (e1->responses.size() != e2->responses.size()) {
    throw incompatible_number_of_event_units()
        << "[ERROR]: The number of responses from two systematic "
           "providers differs, Provider 1 has "
        << e1->responses.size() << ", and 2 has " << e2->responses.size();
    throw;
  }
  size_t NResponses = e1->responses.size();
  for (size_t eur_it = 0; eur_it < NResponses; ++eur_it) {
    for (ParamResponses &resps : e2->responses[eur_it]) {
      if (ContainterHasParamResponses(e1->responses[eur_it], resps.pid)) {
        throw systParamId_collision()
            << "[ERROR]: Failed to insert response of parameter ID = "
            << resps.pid << ", it already exists.";
      }
      e1->responses[eur_it].push_back(std::move(resps));
    }
  }
}
} // namespace larsyst
