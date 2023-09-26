#pragma once

#include "systematicstools/interface/SystParamHeader.hh"
#include "systematicstools/interface/types.hh"

#include "systematicstools/utility/exceptions.hh"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace systtools {

/// \brief Exception raised when no key-value pair with a given key can be found
/// in a given SystParamHeader.
NEW_SYSTTOOLS_EXCEPT(no_such_opt_kv);
/// \brief Exception raised if a SystMetaData fails basic interface validation.
NEW_SYSTTOOLS_EXCEPT(invalid_SystMetaData);
/// \brief A list of Parameter Headers
///
/// Used throughout the interface and interpreters as a 'database' of currently
/// handled systematic parameters.
typedef std::vector<SystParamHeader> SystMetaData;

///\brief Get parameter Id from a SystMetaData and pretty name.
///
/// Returns kParamUnhandled<paramId_t> on failure.
paramId_t GetParamId(SystMetaData const &md, std::string const &name);

///\brief Get parameter index in header list for supplied parameter Id.
///
/// Returns kParamUnhandled<size_t> on failure.
size_t GetParamIndex(SystMetaData const &md, paramId_t pid);

///\brief Whether a given index is handled by the Syst meta data headers.
bool IndexIsHandled(SystMetaData const &md, size_t index);

///\brief Get parameter index in header list for supplied parameter pretty name.
///
/// Returns kParamUnhandled<size_t> on failure.
size_t GetParamIndex(SystMetaData const &md, std::string const &name);

///\brief Checks if named parameter exists in header list.
bool HasParam(SystMetaData const &md, std::string const &name);

///\brief Checks if any of the named parameters exists in header list.
bool HasAnyParams(SystMetaData const &md, std::vector<std::string> const &names);


///\brief Checks if parameter with given Id exists in header list.
bool HasParam(SystMetaData const &md, paramId_t pid);

///\brief Gets a const reference to a parameter header given a header list and a
/// parameter pretty name.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
SystParamHeader const &GetParam(SystMetaData const &md,
                                       std::string const &name);
///\brief Gets a non-const reference to a parameter header given a header list
/// and a parameter pretty name.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
SystParamHeader &GetParam(SystMetaData &md, std::string const &name);

///\brief Gets a const reference to a parameter header given a header list and a
/// parameter Id.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
SystParamHeader const &GetParam(SystMetaData const &md, paramId_t pid);

///\brief Gets a const reference to a parameter header given a header list and a
/// parameter Id.
///
///\note Throws on failure, look before you leap (or prepare a safety net).
SystParamHeader &GetParam(SystMetaData &md, paramId_t pid);

///\brief Returns true if the Parameter Header specified by ident has a matching
/// opts entry.
template <typename T>
inline bool SystHasOpt(SystMetaData const &md, T const &ident,
                       std::string const &opt) {
  if (!HasParam(md, ident)) {
    return false;
  }
  SystParamHeader const &hdr = GetParam(md, ident);
  return (std::find(hdr.opts.begin(), hdr.opts.end(), opt) != hdr.opts.end());
}
///\brief Returns true if the Parameter Header specified by ident has a matching
/// opts key-value entry.
///
/// \note Looks for an entry in SystParamHeader::opts that begins with `<key>=`
template <typename T>
inline bool SystHasOptKV(SystMetaData const &md, T const &ident,
                         std::string const &key) {
  if (!HasParam(md, ident)) {
    return false;
  }
  SystParamHeader const &hdr = GetParam(md, ident);
  for (auto const &opt : hdr.opts) {
    if (opt.find(key + "=") == 0) {
      return true;
    }
  }
  return false;
}
///\brief Returns the option value corresponding to `key` on the Param Header
/// specified by ident.
///
/// \note Looks for an entry in SystParamHeader::opts that begins with `<key>=`
/// and returns the rest of the string.
template <typename T>
inline std::string SystGetOptKV(SystMetaData const &md, T const &ident,
                                std::string const &key) {
  if (!HasParam(md, ident)) {
    return "";
  }
  SystParamHeader const &hdr = GetParam(md, ident);
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
bool Validate(SystMetaData const &sh, bool quiet = true);

///\brief Merges two SystMetaData instances
///
/// The elements of the second parameter list are copied into the first
/// parameter list.
///
/// \note If either fail validation (systtools::invalid_SystMetaData), or if the
/// second set uses a systParamId that is already used by the first set
/// (systtools::systParamId_collision), then an exception is raised.
void ExtendSystMetaData(SystMetaData &md1, SystMetaData const &md2);

} // namespace systtools

