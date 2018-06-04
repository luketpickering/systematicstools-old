#ifndef LARSYST_INTERFACE_VALIDATION_SEEN
#define LARSYST_INTERFACE_VALIDATION_SEEN

#include "SystMetaData.hh"

#include <iomanip>
#include <iostream>
#include <set>

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
inline bool validate_SystParamHeader(SystParamHeader const &hdr,
                                     bool quiet = true) {

  if (hdr.systParamId == kParamUnhandled<paramId_t>) {
    if (!quiet) {
      std::cout << "[ERROR]: SystParamHeader has the default systParamId."
                << std::endl;
    }
    return false;
  }
  if (!hdr.prettyName.size()) {
    if (!quiet) {
      std::cout << "[ERROR]: SystParamHeader doesn't have a prettyName."
                << std::endl;
    }
    return false;
  }
  if (hdr.isCorrection) {
    if (hdr.centralParamValue == 0xdeadb33f) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") is marked as a correction but the centralParamValue is "
                     "defaulted."
                  << std::endl;
      }
      return false;
    }
    if (hdr.paramVariations.size() || hdr.responses.size()) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") is marked as a correction but has variations ("
                  << hdr.paramVariations.size() << ") or responses ("
                  << hdr.responses.size() << ")" << std::endl;
      }
      return false;
    }
  } else {
    if (!hdr.paramVariations.size()) {
      if (!quiet) {
        std::cout
            << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
            << std::quoted(hdr.prettyName)
            << ") is not marked as a correction, but contains no variations."
            << std::endl;
      }
      return false;
    }
  }

  if (hdr.isSplineable) { // Splineable
    if (hdr.isRandomlyThrown) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") marked as splineable is also set as randomly thrown."
                  << std::endl;
      }
      return false;
    }
    if (hdr.isResponselessParam) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") marked as splineable is also set as expressing "
                     "response through another parameter ("
                  << hdr.responseParamId << ")." << std::endl;
      }
      return false;
    }
  }
  if (hdr.isResponselessParam) {
    if (hdr.responses.size()) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") marked as responseless, but also has "
                     "header-level responses."
                  << std::endl;
      }
      return false;
    }
    if (hdr.responseParamId == kParamUnhandled<paramId_t>) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") marked as responseless, but it doesn't have a valid, "
                     "associated response parameter."
                  << std::endl;
      }
      return false;
    }
  }
  if (hdr.differsEventByEvent) { // differs event by event
    if (hdr.responses.size()) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") marked as differing event by event, but also has "
                     "header-level responses."
                  << std::endl;
      }
      return false;
    }
  } else {
    if (!hdr.responses.size()) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") marked as not differing event by event, but has no "
                     "header-level responses."
                  << std::endl;
      }
      return false;
    }
    if (!hdr.isCorrection &&
        (hdr.responses.size() != hdr.paramVariations.size())) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") marked as differing event by event, but also has "
                     "header-level responses."
                  << std::endl;
      }
      return false;
    }
  }
  return true;
}

///\brief Checks for declared and mis-used interdependency between parameters in
/// a list of parameter headers.
///
/// Checks performed:
/// * Are all header parameter Ids unique within the parameter set? (should)
/// * Do the declared response parameter of responless parameters exist within
/// the set? (should)
/// * Do all associated responseless, and the response parameter itself, have
/// the same number of parameter variations? (should, N.B. Can be 0 for
/// corrections.)
inline bool validate_SystMetaData(SystMetaData const &sh, bool quiet = true) {
  std::map<paramId_t, std::vector<paramId_t>> ResponselessParamSets;
  std::set<paramId_t> UsedIds;
  for (auto &hdr : sh.headers) {
    if (!validate_SystParamHeader(hdr, quiet)) {
      return false;
    }
    auto inserted = UsedIds.insert(hdr.systParamId);
    if (!inserted.second) {
      if (!quiet) {
        std::cout << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
                  << std::quoted(hdr.prettyName)
                  << ") has an Id which collides with one already in use."
                  << std::endl;
      }
      return false;
    }
    if (hdr.isResponselessParam) {
      ResponselessParamSets[hdr.responseParamId].push_back(hdr.systParamId);
    }
  }
  for (auto const &rps : ResponselessParamSets) {
    SystParamHeader const &firsthdr = GetParam(sh, rps.second.front());
    if (!HasParam(sh, rps.first)) {
      if (!quiet) {
        std::cout
            << "[ERROR]: SystParamHeader(" << firsthdr.systParamId << ":"
            << std::quoted(firsthdr.prettyName)
            << ") is marked as responseless, and expressing response through "
            << rps.first << ", but no parameter with that Id can be found."
            << std::endl;
      }
      return false;
    }

    SystParamHeader const &responseHdr = GetParam(sh, rps.first);
    size_t NResponseParamVariations = responseHdr.paramVariations.size();
    for (auto const &rp : rps.second) {
      SystParamHeader const &hdr = GetParam(sh, rp);
      if (NResponseParamVariations != hdr.paramVariations.size()) {
        if (!quiet) {
          std::cout
              << "[ERROR]: SystParamHeader(" << hdr.systParamId << ":"
              << std::quoted(hdr.prettyName)
              << ") is marked as responseless, and expressing response through "
              << responseHdr.systParamId << ":"
              << std::quoted(responseHdr.prettyName)
              << ", but they have a differing number of parameter variations: "
              << hdr.paramVariations.size()
              << " != " << NResponseParamVariations << ", respectively."
              << std::endl;
        }
        return false;
      }
    }
  }
  return true;
}

} // namespace larsyst
#endif
