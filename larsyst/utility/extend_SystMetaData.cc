#include "extend_SystMetaData.hh"

#include "larsyst/interface/validation.hh"

#include "larsyst/utility/exceptions.hh"
#include "larsyst/utility/printers.hh"

#include <set>

NEW_LARSYST_EXCEPT(invalid_SystMetaData);

namespace larsyst {
void extend_SystMetaData(SystMetaData &md1, SystMetaData const &md2) {
  if (!validate_SystMetaData(md1, false) ||
      !validate_SystMetaData(md2, false)) {
    throw invalid_SystMetaData()
        << "[ERROR]: Attempting to merge two parameter header sets but "
           "one (md1 is valid:"
        << validate_SystMetaData(md1)
        << ", md2 is valid:" << validate_SystMetaData(md2) << ") is invalid.";
  }

  std::set<paramId_t> UsedIds;
  for (auto const &sph : md1) {
    // No colliding ids in each parameter set already checked by
    // validate_SystMetaData
    UsedIds.insert(sph.systParamId);
  }

  for (auto const &sph : md2) {
    auto inserted = UsedIds.insert(sph.systParamId);
    if (!inserted.second) {
      throw systParamId_collision()
          << "[ERROR]: Attempting to merge two parameter header sets, "
             "but a parameter Id collison has been found for Id: "
          << std::quoted(sph.systParamId);
    }
  }

  for (auto const &sph : md2) {
    md1.push_back(sph);
  }
}
} // namespace larsyst
