#ifndef LARSYST_INTERFACE_SYSTMETADATA_SEEN
#define LARSYST_INTERFACE_SYSTMETADATA_SEEN

#include "larsyst/interface/SystParamHeader.hh"
#include "larsyst/interface/types.hh"

#include "larsyst/utility/exceptions.hh"
#include "larsyst/utility/printers.hh"

#include <array>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace larsyst {

typedef std::vector<SystParamHeader> SystMetaData;

///\brief Get parameter Id from header list and pretty name. Returns
/// kParamUnhandled<paramId_t> on failure.
inline paramId_t GetParamId(SystMetaData const &md, std::string const &name) {
  for (size_t it = 0; it < md.size(); ++it) {
    if (md[it].prettyName == name) {
      return md[it].systParamId;
    }
  }
  return kParamUnhandled<paramId_t>;
}
///\brief Get parameter index in header list for supplied parameter Id. Returns
/// kParamUnhandled<size_t> on failure.
inline size_t GetParamIndex(SystMetaData const &md, paramId_t pid) {
  for (size_t it = 0; it < md.size(); ++it) {
    if (md[it].systParamId == pid) {
      return it;
    }
  }
  return kParamUnhandled<size_t>;
}
///\brief Whether a given index is handled by the Syst meta data headers.
inline bool IndexIsHandled(SystMetaData const &md, size_t index) {
  return (index != kParamUnhandled<size_t>)&&(index < md.size());
}
///\brief Get parameter index in header list for supplied parameter pretty name.
/// Returns kParamUnhandled<size_t> on failure.
inline size_t GetParamIndex(SystMetaData const &md, std::string const &name) {
  return GetParamIndex(md, GetParamId(md, name));
}
///\brief Checks if named parameter exists in header list.
inline bool HasParam(SystMetaData const &md, std::string const &name) {
  return IndexIsHandled(md, GetParamIndex(md, name));
}
///\brief Checks if parameter with given Id exists in header list.
inline bool HasParam(SystMetaData const &md, paramId_t pid) {
  return IndexIsHandled(md, GetParamIndex(md, pid));
}
///\brief Gets a const reference to a parameter header given a header list and a
/// parameter pretty name.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
inline SystParamHeader const &GetParam(SystMetaData const &md,
                                       std::string const &name) {
  size_t idx = GetParamIndex(md, name);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_name_not_handled()
      << "[ERROR]: Tried to get parameter named " << std::quoted(name)
      << " from a larsyst::SystMetaData instance, but it doesn't exist.";
}
///\brief Gets a non-const reference to a parameter header given a header list
/// and a parameter pretty name.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
inline SystParamHeader &GetParam(SystMetaData &md, std::string const &name) {
  size_t idx = GetParamIndex(md, name);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_name_not_handled()
      << "[ERROR]: Tried to get parameter named " << std::quoted(name)
      << " from a larsyst::SystMetaData instance, but it doesn't exist.";
}
///\brief Gets a const reference to a parameter header given a header list and a
/// parameter Id.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
inline SystParamHeader const &GetParam(SystMetaData const &md, paramId_t pid) {
  size_t idx = GetParamIndex(md, pid);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_Id_not_handled()
      << "[ERROR]: Tried to get parameter with id " << std::quoted(pid)
      << " from a SystMetaData instance, but it doesn't exist.";
}
///\brief Gets a const reference to a parameter header given a header list and a
/// parameter Id.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
inline SystParamHeader &GetParam(SystMetaData &md, paramId_t pid) {
  size_t idx = GetParamIndex(md, pid);
  if (IndexIsHandled(md, idx)) {
    return md[idx];
  }
  throw parameter_Id_not_handled()
      << "[ERROR]: Tried to get parameter with id " << std::quoted(pid)
      << " from a SystMetaData instance, but it doesn't exist.";
}
template <typename T>
inline bool SystHasOpt(SystMetaData const &md, T const &pid,
                       std::string const &opt) {
  if (!HasParam(md, pid)) {
    return false;
  }
  SystParamHeader const &hdr = GetParam(md, pid);
  return (std::find(hdr.opts.begin(), hdr.opts.end(), opt) != hdr.opts.end());
}
template <typename T>
inline bool SystHasOptKV(SystMetaData const &md, T const &pid,
                         std::string const &key) {
  if (!HasParam(md, pid)) {
    return false;
  }
  SystParamHeader const &hdr = GetParam(md, pid);
  for (auto const &opt : hdr.opts) {
    if (opt.find(key + "=") == 0) {
      return true;
    }
  }
  return false;
}

NEW_LARSYST_EXCEPT(no_such_opt_kv);

template <typename T>
inline std::string SystGetOptKV(SystMetaData const &md, T const &pid,
                                std::string const &key) {
  if (!HasParam(md, pid)) {
    return "";
  }
  SystParamHeader const &hdr = GetParam(md, pid);
  for (auto const &opt : hdr.opts) {
    if (opt.find(key + "=") == 0) {
      return opt.substr(key.size() + 1);
    }
  }
  throw no_such_opt_kv() << "[ERROR]: For header, "
                         << std::quoted(hdr.prettyName)
                         << " failed to find KV option for key: "
                         << std::quoted(key);
}

NEW_LARSYST_EXCEPT(invalid_SystMetaData);

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
inline bool Validate(SystMetaData const &sh, bool quiet = true) {
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

inline void ExtendSystMetaData(SystMetaData &md1, SystMetaData const &md2) {
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
             "but a parameter Id collison has been found for Id: "
          << std::quoted(sph.systParamId);
    }
  }

  for (auto const &sph : md2) {
    md1.push_back(sph);
  }
}

} // namespace larsyst

#endif
