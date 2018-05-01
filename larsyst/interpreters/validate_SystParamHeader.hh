#ifndef LARSYST_INTERPRETERS_VALIDATESYSTPARAMHEADER_SEEN
#define LARSYST_INTERPRETERS_VALIDATESYSTPARAMHEADER_SEEN

#include "larsyst/interface/SystMetaData.hh"

namespace larsyst {
///\brief Checks interface validity of a SystParamHeader
///
/// Checks performed:
/// * Has valid Id
/// * Has non-empty pretty name
/// * If it is a correction:
///  * Does it have a specified central value? (should)
///  * Does it have any responses or parameter variations defined? (shouldn't)
/// * If it is not a correction, does it have at least one parameter variation
/// specified?
/// * If it is marked as splineable:
///  * Is it also marked as randomly thrown? (shouldn't)
///  * Is it also marked as responseless? (shouldn't)
/// * If it is marked as responseless:
///  * Does it have a corresponding response parameter? (should)
///  * Does it have any responses defined? (shouldn't)
/// * If it is marked as not differing event-by-event:
///  * Does it have header-level responses defined? (should)
///  * Does it have parameter variations specified? (should unless marked as a
///  correction)
/// * If it is marked as differing event-by-event, does it have header-level
/// responses defined? (shouldn't)
bool validate_SystParamHeader(SystParamHeader const &, bool quiet = true);
} // namespace larsyst

#endif
