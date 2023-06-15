#pragma once

#include "systematicstools/interface/SystParamHeader.hh"
#include "systematicstools/interface/types.hh"

#include "systematicstools/utility/exceptions.hh"

#include <memory>
#include <vector>

namespace systtools {

/// Struct for holding parameter--response associations
///
///\note Kept distinct from ParamThrows to preserve the important semantic
/// difference of the two, even if the data structures are identical.
///
/// Gives semantic meaning to what might otherwise be implemented as a
/// std::pair.
struct ParamResponses {
  paramId_t pid;
  std::vector<double> responses;
};
typedef std::vector<ParamResponses> event_unit_response_t;

///\brief The systematic parameter responses calculated for an event.
///
/// For each 'object of interest' (e.g. neutrino interaction, muon track, ...)
/// within an event, the relevant responses to parameter variations are stored.
///
///\note Use systtools::SystParamHeader and systtools::ParamHeaderHelper for
/// response interpretation tools
typedef std::vector<event_unit_response_t> EventResponse;

struct VarAndCVResponse {
  systtools::paramId_t pid;
  double CV_response;
  std::vector<double> responses;
};
typedef std::vector<VarAndCVResponse> event_unit_response_w_cv_t;

typedef std::vector<event_unit_response_w_cv_t> EventAndCVResponse;

///\brief Exception raised when attempting to merge two event responses with
/// differing number of event units.
NEW_SYSTTOOLS_EXCEPT(incompatible_number_of_event_units);

///\brief Extends one EventUnitResponse with the parameter responses of another.
///
/// The parameter responses from e2 are moved into e1.
template <class EUR> void ExtendEventUnitResponse(EUR &e1, EUR &&e2) {

  for (typename EUR::value_type &resps : e2) {
    if (ContainterHasParam(e1, resps.pid)) {
      throw systParamId_collision()
          << "[ERROR]: Failed to insert response of parameter ID = "
          << resps.pid << ", it already exists.";
    }
    e1.push_back(std::move(resps));
  }
}

///\brief Extends one EventResponse with the event_unit_response_ts of another.
///
/// The sizes of each EventResponse must be the same or a
/// incompatible_number_of_event_units exception will be raised.
///
/// The event_unit_response_ts from e2 are moved into e1.
template <class ER>
void ExtendEventResponse(std::unique_ptr<ER> &e1, std::unique_ptr<ER> &&e2) {

  if (!e1 || !e2) {
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
    ExtendEventUnitResponse(e1->at(eur_it), std::move(e2->at(eur_it)));
  }
}

/// \brief Removes systtools::ParamResponses from event_unit_response_ts
/// contained within an EventResponse that contain only unity responses.
///
/// \note that this is intended to be applied to weight systematics that do not
/// affect a given event
void ScrubUnityEventResponses(std::unique_ptr<EventResponse> &er);

/// \brief Removes systtools::ParamResponses from event_unit_response_t that
/// contain only unity responses.
///
/// \note that this is intended to be applied to weight systematics that do not
/// affect a given event
void ScrubUnityEventResponses(event_unit_response_t &er);

} // namespace systtools
