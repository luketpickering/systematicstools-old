#ifndef EVENTSYST_PRODUCT_HXX_SEEN
#define EVENTSYST_PRODUCT_HXX_SEEN

#include <vector>
#include <map>

namespace larsyst {
struct EventResponse {
  ///\brief The systematic parameter responses calculated for an ART event.
  ///
  /// For each 'object of interest' (e.g. neutrino interaction) within an event,
  /// a map of relevant parameter responses is stored. The char key relates the
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
  std::vector<std::map<char, std::vector<double>>> responses;
};
} // namespace larsyst

#endif
