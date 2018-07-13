#ifndef LARSYST_INTERFACE_EVENTRESPONSEPRODUCE_SEEN
#define LARSYST_INTERFACE_EVENTRESPONSEPRODUCE_SEEN

#include "larsyst/interface/ParamResponses.hh"
#include "larsyst/interface/types.hh"

#include <memory>

namespace larsyst {

///\brief The systematic parameter responses calculated for an ART event.
///
/// For each 'object of interest' (e.g. neutrino interaction) within an event,
/// a map of relevant parameter responses is stored. The uint key relates the
/// parameter information to the header through correspondence with the
/// larsyst::systParamHeader::systParamId data member.
///
/// The responses contained within the inner `std::vector<double>` must be
/// interpreted differently based upon the corresponding values of
/// `larsyst::systParamHeader::isWeightSystematicVariation` and
/// `larsyst::systParamHeader::isSplineable`. An empty inner vector signifies
/// than an object is relevant for the corresponding parameter, but all of the
/// response information can be determined from the
/// `larsyst::systParamHeader`.
typedef std::vector<event_unit_response_t> EventResponse;

///\brief Exception raised when attempting to merge two event responses with
/// differing number of event units.
NEW_LARSYST_EXCEPT(incompatible_number_of_event_units);

///\brief Extends one EventResponse with the event_unit_response_ts of another.
///
/// The sizes of each EventResponse must be the same or a incompatible_number_of_event_units exception will be raised.
///
/// The event_unit_response_ts from e2 are moved into e1.
inline void ExtendEventResponse(std::unique_ptr<EventResponse> &e1,
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
      if (ContainterHasParamResponses(e1->at(eur_it), resps.pid)) {
        throw systParamId_collision()
            << "[ERROR]: Failed to insert response of parameter ID = "
            << resps.pid << ", it already exists.";
      }
      e1->at(eur_it).push_back(std::move(resps));
    }
  }
}

} // namespace larsyst

#endif
