#include "append_event_response.hh"

#include <iostream>

namespace larsyst {
void append_event_response(std::unique_ptr<larsyst::EventResponse> &e1,
                           std::unique_ptr<larsyst::EventResponse> &&e2) {

  if (!e2) {
    return;
  }

  if (e1->responses.size() != e2->responses.size()) {
    std::cout << "[ERROR]: The number of responses from two systematic "
                 "providers differs, Provider 1 has "
              << e1->responses.size() << ", and 2 has " << e2->responses.size()
              << std::endl;
    throw;
  }
  for (size_t p_it = 0; p_it < e1->responses.size(); ++p_it) {
    for (auto &param_response : e2->responses[p_it]) {
      auto inserted = e1->responses[p_it].insert(std::move(param_response));
      if (!inserted.second) {
        std::cout << "[ERROR]: Failed to insert response of parameter ID = "
                  << param_response.first << ", it already exists."
                  << std::endl;
        throw;
      }
    }
  }
}
} // namespace larsyst
