#include "systematicstools/utility/ParameterAndProviderConfigurationUtility.hh"

#include "systematicstools/utility/FHiCLSystParamHeaderUtility.hh"

#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

namespace systtools {

param_header_map_t BuildParameterHeaders(fhicl::ParameterSet const &paramset,
                                         std::string const &key) {

  param_header_map_t headers;

  // Foreach provider block
  auto const &provider_keys = paramset.get<std::vector<std::string>>(key);
  for (auto const &provkey : provider_keys) {
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provkey);

    std::string provname = provider_cfg.get<std::string>("tool_type");
    if (provider_cfg.has_key("instance_name")) {
      provname += "_" + provider_cfg.get<std::string>("instance_name");
    }

    std::vector<std::string> const &ParameterHeaderKeyNames =
        provider_cfg.get<std::vector<std::string>>("parameter_headers");

    // Foreach handled parameter block
    for (auto const &ParamHeaderKey : ParameterHeaderKeyNames) {
      SystParamHeader hdr = FHiCLToSystParamHeader(
          provider_cfg.get<fhicl::ParameterSet>(ParamHeaderKey));

      // Check that this unique Id hasn't been used before.
      if (headers.find(hdr.systParamId) != headers.end()) {
        throw systParamId_collision()
            << "[ERROR]:\t Header describing parameter " << hdr.systParamId
            << " already exists (provider: "
            << std::quoted(headers[hdr.systParamId].ProviderFQName)
            << ", prettyName: "
            << std::quoted(headers[hdr.systParamId].Header.prettyName)
            << ")"
            << ", this parameter: { provider: " << std::quoted(provname)
            << ", prettyName: " << std::quoted(hdr.prettyName)
            << "} cannot be added. ";
      }

      headers.emplace(param_header_map_t::key_type{hdr.systParamId},
                      param_header_map_t::mapped_type{provname, hdr});
    }
  }

  return headers;
}
} // namespace systtools
