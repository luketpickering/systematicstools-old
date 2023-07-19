#pragma once

#include "systematicstools/interface/EventResponse_product.hh"
#include "systematicstools/interface/SystMetaData.hh"
#include "systematicstools/interface/SystParamHeader.hh"

#include "systematicstools/utility/exceptions.hh"

#include <map>
#include <memory>
#include <vector>

namespace systtools {

class ISystProviderTool;

/// Struct for holding parameter--value associations
///
/// Gives semantic meaning to what might otherwise be implemented as a
/// std::pair.
struct ParamValue {
  paramId_t pid;
  double val;
};

/// List of parameter--value associations
///
/// Useful for handling 'state' in an analysis that is sampling a
/// parameter-space.
typedef std::vector<ParamValue> param_value_list_t;

typedef std::vector<paramId_t> param_list_t;

/// Struct for holding parameter--thrown value associations
///
/// Gives semantic meaning to what might otherwise be implemented as a
/// std::pair.
struct ParamThrows {
  paramId_t pid;
  std::vector<double> thrown_vals;
};

/// List of parameter--thrown value associations
///
/// Useful for tracking parameter values of multi-universe error propagation
/// approaches.
typedef std::vector<ParamThrows> parameter_throws_list_t;

typedef std::vector<std::unique_ptr<ISystProviderTool>>
    provider_list_t;

///\brief Struct for holding ISystProviderTool unique name--handled parameter
/// header pairs.
///
/// Gives semantic meaning to what might otherwise be implemented as a
/// std::pair.
struct ParamHeaderProviderName {
  std::string ProviderFQName;
  SystParamHeader Header;
};

///\brief  Map of parameter Identifiers to the relevant metadata and the unique
/// name of the ISystProviderTool responsible for generating them.
typedef std::map<paramId_t, ParamHeaderProviderName> param_header_map_t;

///\brief Gets the index of a parameter--X association with a given paramId_t
///
/// Returns kParamUnhandled<size_t> if parameter does not exist in the
/// list
///
/// Useful for interacting with: param_value_list_t,
/// parameter_throws_list_t, and
template <typename T>
size_t GetParamContainerIndex(std::vector<T> const &container, paramId_t pid) {
  size_t NVals = container.size();
  for (size_t i = 0; i < NVals; ++i) {
    if (container[i].pid == pid) {
      return i;
    }
  }
  return kParamUnhandled<size_t>;
}

/// Checks whether a parameter with paramId_t == pid is contained
///
/// Uses GetParamContainerIndex and so will work with any types that that method
/// does.
template <typename T>
inline bool ContainterHasParam(std::vector<T> const &container, paramId_t pid) {
  return (GetParamContainerIndex(container, pid) != kParamUnhandled<size_t>);
}

/// Gets a reference to a contained element with paramId_t == pid.
///
/// \note throws for non-contained elements. Look before you leap.
template <typename T>
inline T &GetParamElementFromContainer(std::vector<T> &container,
                                       paramId_t pid) {
  for (T &el : container) {
    if (el.pid == pid) {
      return el;
    }
  }
  throw invalid_parameter_Id();
}

/// Gets a const reference to a contained element with paramId_t == pid.
///
/// \note throws for non-contained elements. Look before you leap.
template <typename T>
inline T const &GetParamElementFromContainer(std::vector<T> const &container,
                                             paramId_t pid) {
  for (T const &el : container) {
    if (el.pid == pid) {
      return el;
    }
  }
  throw invalid_parameter_Id();
}

} // namespace systtools
