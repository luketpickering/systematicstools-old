#include "systematicstools/interface/SystParamHeader.hh"

#include <iomanip>
#include <iostream>

namespace systtools {
bool Validate(SystParamHeader const &hdr, bool quiet) {

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
    if (hdr.centralParamValue == kDefaultDouble) {
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
}
