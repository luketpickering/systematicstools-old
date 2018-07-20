#include "larsyst/interface/SystMetaData.hh"

#include <set>

namespace larsyst {

paramId_t GetParamId(SystMetaData const &md, std::string const &name) {
  for (size_t it = 0; it < md.size(); ++it) {
    if (md[it].prettyName == name) {
      return md[it].systParamId;
    }
  }
  return kParamUnhandled<paramId_t>;
}

size_t GetParamIndex(SystMetaData const &md, paramId_t pid) {
  for (size_t it = 0; it < md.size(); ++it) {
    if (md[it].systParamId == pid) {
      return it;
    }
  }
  return kParamUnhandled<size_t>;
}

bool IndexIsHandled(SystMetaData const &md, size_t index) {
  return (index != kParamUnhandled<size_t>)&&(index < md.size());
}

size_t GetParamIndex(SystMetaData const &md, std::string const &name) {
  return GetParamIndex(md, GetParamId(md, name));
}

bool HasParam(SystMetaData const &md, std::string const &name) {
  return IndexIsHandled(md, GetParamIndex(md, name));
}

bool HasParam(SystMetaData const &md, paramId_t pid) {
  return IndexIsHandled(md, GetParamIndex(md, pid));
}

SystParamHeader const &GetParam(SystMetaData const &md,
                                       std::string const &name) {
  size_t idx = GetParamIndex(md, name);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_name_not_handled()
      << "[ERROR]: Tried to get parameter named " << std::quoted(name)
      << " from a larsyst::SystMetaData instance, but it doesn't exist.";
}

SystParamHeader &GetParam(SystMetaData &md, std::string const &name) {
  size_t idx = GetParamIndex(md, name);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_name_not_handled()
      << "[ERROR]: Tried to get parameter named " << std::quoted(name)
      << " from a larsyst::SystMetaData instance, but it doesn't exist.";
}

SystParamHeader const &GetParam(SystMetaData const &md, paramId_t pid) {
  size_t idx = GetParamIndex(md, pid);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_Id_not_handled()
      << "[ERROR]: Tried to get parameter with id \"" << pid
      << "\" from a SystMetaData instance, but it doesn't exist.";
}

SystParamHeader &GetParam(SystMetaData &md, paramId_t pid) {
  size_t idx = GetParamIndex(md, pid);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_Id_not_handled()
      << "[ERROR]: Tried to get parameter with id \"" << pid
      << "\" from a SystMetaData instance, but it doesn't exist.";
}

bool Validate(SystMetaData const &sh, bool quiet) {
  std::map<paramId_t, std::vector<paramId_t>> ResponselessParamSets;
  std::set<paramId_t> UsedIds;
  for (auto &hdr : sh) {
    if (!Validate(hdr, quiet)) {
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

void ExtendSystMetaData(SystMetaData &md1, SystMetaData const &md2) {
  if (!Validate(md1, false) || !Validate(md2, false)) {
    throw invalid_SystMetaData()
        << "[ERROR]: Attempting to merge two parameter header sets but "
           "one (md1 is valid:"
        << Validate(md1) << ", md2 is valid:" << Validate(md2)
        << ") is invalid.";
  }

  std::set<paramId_t> UsedIds;
  for (auto const &sph : md1) {
    // No colliding ids in each parameter set already checked by
    // Validate(md1)
    UsedIds.insert(sph.systParamId);
  }

  for (auto const &sph : md2) {
    auto inserted = UsedIds.insert(sph.systParamId);
    if (!inserted.second) {
      throw systParamId_collision()
          << "[ERROR]: Attempting to merge two parameter header sets, "
             "but a parameter Id collison has been found for Id: \""
          << sph.systParamId << "\"";
    }
  }

  for (auto const &sph : md2) {
    md1.push_back(sph);
  }
}
} // namespace larsyst
