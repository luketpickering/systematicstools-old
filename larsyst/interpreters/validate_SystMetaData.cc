#include "validate_SystMetaData.hh"
#include "validate_SystParamHeader.hh"

namespace larsyst {
bool validate_SystMetaData(SystMetaData const &sh, bool quiet = true) {
  std::map<paramId_t, std::vector<paramId_t>> ResponselessParamSets;
  for (auto &hdr : sh.headers) {
    if (!validate_SystParamHeader(hdr, quiet)) {
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
              << std : quoted(responseHdr.prettyName)
              << ", but they have a differing number of parameter variations: "
              << hdr.paramVariations.size()
              << " != "
              << NResponseParamVariations
              << ", respectively."
              << std::endl;
        }
        return false;
      }
    }
  }
  return true;
}
} // namespace larsyst
