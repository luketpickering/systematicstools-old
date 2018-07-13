#ifndef LARSYST_INTERFACE_PARAMRESPONSES_SEEN
#define LARSYST_INTERFACE_PARAMRESPONSES_SEEN

#include "larsyst/interface/types.hh"

#include "larsyst/utility/exceptions.hh"

#include <vector>

namespace larsyst {
struct ParamResponses {
  paramId_t pid;
  std::vector<double> responses;
};

template <typename T>
inline bool ContainterHasParamResponses(T const &container, paramId_t pid) {
  for (ParamResponses const &pr : container) {
    if (pr.pid == pid) {
      return true;
    }
  }
  return false;
}

template <typename T>
inline ParamResponses &GetParamResponsesContainer(T &container, paramId_t pid) {
  for (ParamResponses &pr : container) {
    if (pr.pid == pid) {
      return pr;
    }
  }
  throw invalid_parameter_Id();
}
template <typename T>
inline ParamResponses const &GetParamResponsesContainer(T const &container,
                                                        paramId_t pid) {
  for (ParamResponses const &pr : container) {
    if (pr.pid == pid) {
      return pr;
    }
  }
  throw invalid_parameter_Id();
}
} // namespace larsyst

#endif
