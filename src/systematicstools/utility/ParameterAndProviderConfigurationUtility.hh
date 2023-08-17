#pragma once

#include "systematicstools/interface/ISystProviderTool.hh"
#include "systematicstools/interface/SystParamHeader.hh"
#include "systematicstools/interface/types.hh"

#include "systematicstools/utility/exceptions.hh"

#include "fhiclcpp/ParameterSet.h"

#include <chrono>
#include <functional>
#include <memory>
#include <random>
#include <string>

namespace systtools {

///\brief Exception thrown when two ISystProviderTools have identical fully
/// qualified (tool_name + instance_name) names.
NEW_SYSTTOOLS_EXCEPT(ISystProvider_FQName_collision);

///\brief Builds map of SystProvider instance names and handled parameters from
/// a ParameterHeaders FHiCL document.
///
/// Used by standalone interpreters to read response interpretation metadata
/// from input FHiCL
param_header_map_t
BuildParameterHeaders(fhicl::ParameterSet const &paramset,
                      std::string const &key = "syst_providers");

///\brief Builds map of SystProvider instances and handled parameters from a
/// set of pre-configured providers
///
/// Avoids reading the same FHiCL twice!
template <typename T = systtools::ISystProviderTool>
param_header_map_t BuildParameterHeaders(
    std::vector<std::unique_ptr<T>> const &ConfiguredProviders) {
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

///\brief Configures the set of ISystProviders from a Tool Configuration
/// document.
///
/// Some structure over the paramset is neccessary (and is described in
/// systematicstools/doc/ToolConfiguration.md ), but the FHiCL document passed
/// to InstanceBuild is tool sub-class-specific. This is as opposed to
/// ConfigureISystProvidersFromParameterHeaders which requires a rigidly
/// structure document.
///
/// The InstanceBuilder function argument is used to instantiate
/// ISystProviderTool instances held by std::unique_ptrs. When running with
/// ART support this will default to
/// art::make_tool<systtools::ISystProviderTool>, but when running outside of
/// art, other instantiators must be used.
template <typename T = systtools::ISystProviderTool>
std::vector<std::unique_ptr<T>> ConfigureISystProvidersFromToolConfig(
    fhicl::ParameterSet const &paramset,
    std::function<std::unique_ptr<T>(fhicl::ParameterSet const &)> InstanceBuilder,
    std::string const &key = "syst_providers", paramId_t syst_param_id = 0) {

  // Instantiate RNGs for seed suggestion.
  std::mt19937_64 generator(
      std::chrono::steady_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<uint64_t> distribution(0, 1E6);
  auto RNJesus = std::bind(distribution, generator);

  std::vector<std::unique_ptr<T>> providers;

  for (auto const &provkey : paramset.get<std::vector<std::string>>(key)) {
    // Get fhicl config for provider
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provkey);

    // Make an instance of the plugin
    std::unique_ptr<T> is = InstanceBuilder(provider_cfg);

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

///\brief Configures the set of ISystProviders from a Parameter Headers
/// document.
///
/// The structure of paramset must adhere to the Parameter Headers structure
/// described in systematicstools/doc/ParameterHeaders.md
///
/// The InstanceBuilder function argument is used to instantiate
/// ISystProviderTool instances held by std::unique_ptrs. When running with
/// ART support this will default to
/// art::make_tool<systtools::ISystProviderTool>, but when running outside of
/// art, other instantiators must be used.
template <typename T = systtools::ISystProviderTool>
std::vector<std::unique_ptr<T>> ConfigureISystProvidersFromParameterHeaders(
    fhicl::ParameterSet const &paramset,
    std::function<std::unique_ptr<T>(fhicl::ParameterSet const &)> InstanceBuilder,
    std::string const &key = "syst_providers") {

  std::vector<std::unique_ptr<T>> providers;

  for (auto const &provkey : paramset.get<std::vector<std::string>>(key)) {
    // Get fhicl config for provider
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provkey);

    // Make an instance of the plugin
    std::unique_ptr<T> is = InstanceBuilder(provider_cfg);

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

} // namespace systtools
