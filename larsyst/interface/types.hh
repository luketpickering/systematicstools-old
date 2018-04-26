#ifndef LARSYST_INTERFACE_TYPES_SEEN
#define LARSYST_INTERFACE_TYPES_SEEN

#include <limits>
#include <map>
#include <memory>
#include <vector>

namespace larsyst {

class ISystProvider_tool;
struct SystParamHeader;

typedef unsigned paramId_t;

template <typename T>
constexpr T kParamUnhandled = std::numeric_limits<T>::max();

typedef std::map<paramId_t, double> param_value_map_t;

typedef std::vector<paramId_t> param_list_t;

typedef std::map<paramId_t, std::vector<double>> parameter_throws_list_t;

typedef std::map<std::string, std::unique_ptr<ISystProvider_tool>>
    provider_map_t;

typedef std::map<paramId_t, std::pair<std::string, SystParamHeader>>
    param_header_map_t;

typedef std::map<paramId_t, std::vector<double>> event_unit_response_t;

typedef std::vector<event_unit_response_t> event_response_t;

} // namespace larsyst

#endif
