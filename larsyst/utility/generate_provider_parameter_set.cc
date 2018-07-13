#include "generate_provider_parameter_set.hh"

#include "larsyst/interface/ISystProvider_tool.hh"

#include "build_parameter_set_from_header.hh"

#include <iostream>
#include <vector>

namespace larsyst {

fhicl::ParameterSet
generate_provider_parameter_set(provider_map_t::mapped_type const &provider) {
  return generate_provider_parameter_set(
      std::pair<std::pair<std::string, std::string>,
                std::vector<SystParamHeader>>{
          {provider->GetToolType(), provider->GetInstanceName()},
          provider->GetSystSetConfiguration()});
}

fhicl::ParameterSet generate_provider_parameter_set(
    std::pair<std::pair<std::string, std::string>,
              std::vector<SystParamHeader>> const &ProviderParameterConfig) {
  fhicl::ParameterSet provider_ps;

  std::vector<std::string> parameterHeaderNames;
  for (auto &hdr : ProviderParameterConfig.second) {
    provider_ps.put(hdr.prettyName, build_parameter_set_from_header(hdr));
    parameterHeaderNames.push_back(hdr.prettyName);
  }
  provider_ps.put("parameterHeaders", parameterHeaderNames);
  provider_ps.put("tool_type", ProviderParameterConfig.first.first);
  if (ProviderParameterConfig.first.second.size()) {
    provider_ps.put("uniqueName", ProviderParameterConfig.first.second);
  }

  return provider_ps;
}

} // namespace larsyst
