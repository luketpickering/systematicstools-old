#ifndef LARSYST_INTERFACE_EVENTRESPONSEPRODUCE_SEEN
#define LARSYST_INTERFACE_EVENTRESPONSEPRODUCE_SEEN

#include "larsyst/interface/types.hh"

#include <map>
#include <vector>

namespace larsyst {
struct EventResponse {
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
  event_response_t responses;
};

} // namespace larsyst

#endif
