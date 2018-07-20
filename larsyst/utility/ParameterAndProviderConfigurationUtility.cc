#include "larsyst/utility/ParameterAndProviderConfigurationUtility.hh"

#include "larsyst/interface/ISystProvider_tool.hh"
#include "larsyst/interface/SystParamHeader.hh"

#include "larsyst/utility/FHiCLSystParamHeaderConverters.hh"
#include "larsyst/utility/exceptions.hh"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <vector>

namespace larsyst {

NEW_LARSYST_EXCEPT(ISystProvider_FQName_collision);

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

param_header_map_t
BuildParameterHeaders(provider_list_t const &ConfiguredProviders) {
  param_header_map_t headers;

  for (auto const &provider : ConfiguredProviders) {
    for (auto const &hdr : provider->GetSystMetaData()) {
      headers.emplace(param_header_map_t::key_type{hdr.systParamId},
                      param_header_map_t::mapped_type{
                          provider->GetFullyQualifiedName(), hdr});
    }
  }

  return headers;
}

provider_list_t ConfigureISystProvidersFromToolConfig(
    fhicl::ParameterSet const &paramset,
    std::function<std::unique_ptr<larsyst::ISystProvider_tool>(
        fhicl::ParameterSet const &)>
        InstanceBuilder,
    std::string const &key, paramId_t syst_param_id) {

  // Instantiate RNGs for seed suggestion.
  std::mt19937_64 generator(
      std::chrono::steady_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<uint64_t> distribution(0, 1E6);
  auto RNJesus = std::bind(distribution, generator);

  provider_list_t providers;

  for (auto const &provkey : paramset.get<std::vector<std::string>>(key)) {
    // Get fhicl config for provider
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provkey);

    // Make an instance of the plugin
    std::unique_ptr<larsyst::ISystProvider_tool> is =
        InstanceBuilder(provider_cfg);

    // Suggest a seed
    is->SuggestSeed(RNJesus());
    // Configure the instance
    is->ConfigureFromToolConfig(provider_cfg, syst_param_id);
    SystMetaData md = is->GetSystMetaData();
    syst_param_id += md.size();

    // build unique name
    std::string FQName = is->GetFullyQualifiedName();

    // check that this unique name hasn't been used before.
    for (auto const &prov : providers) {
      if (prov->GetFullyQualifiedName() == FQName) {
        throw ISystProvider_FQName_collision()
            << "[ERROR]:\t Provider with that name already exists, please "
               "correct provider set (Hint: Use the 'unique_name' property "
               "of the tool configuration table to disambiguate multiple "
               "uses of the same tool).";
      }
    }
    providers.emplace_back(std::move(is));
  }
  return providers;
}

provider_list_t ConfigureISystProvidersFromParameterHeaders(
    fhicl::ParameterSet const &paramset,
    std::function<std::unique_ptr<larsyst::ISystProvider_tool>(
        fhicl::ParameterSet const &)>
        InstanceBuilder,
    std::string const &key, paramId_t syst_param_id) {

  provider_list_t providers;

  for (auto const &provkey : paramset.get<std::vector<std::string>>(key)) {
    // Get fhicl config for provider
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provkey);

    // Make an instance of the plugin
    std::unique_ptr<larsyst::ISystProvider_tool> is =
        InstanceBuilder(provider_cfg);

    is->ConfigureFromParameterHeaders(provider_cfg);
    SystMetaData md = is->GetSystMetaData();

    // build unique name
    std::string FQName = is->GetFullyQualifiedName();

    // check that this unique name hasn't been used before.
    for (auto const &prov : providers) {
      if (prov->GetFullyQualifiedName() == FQName) {
        throw ISystProvider_FQName_collision()
            << "[ERROR]:\t Provider with that name already exists, please "
               "correct provider set (Hint: Use the 'unique_name' property "
               "of the tool configuration table to disambiguate multiple "
               "uses of the same tool).";
      }
    }
    providers.emplace_back(std::move(is));
  }
  return providers;
}
} // namespace larsyst
