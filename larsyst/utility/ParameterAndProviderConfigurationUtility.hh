#ifndef LARSYST_INTERPRETERS_LOADPARAMETERHEADERS_SEEN
#define LARSYST_INTERPRETERS_LOADPARAMETERHEADERS_SEEN

#include "larsyst/interface/types.hh"

#include "fhiclcpp/ParameterSet.h"

#include <functional>
#include <memory>
#include <string>

namespace larsyst {

///\brief Builds map of SystProvider instances and handled parameters from a
/// ParameterHeaders FHiCL document.
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
param_header_map_t
BuildParameterHeaders(provider_list_t const &ConfiguredProviders);

///\brief Configures the set of ISystProviders from a Tool Configuration
/// document.
///
/// Some structure over the paramset is neccessary (and is described in
/// larsyst/doc/ToolConfiguration.md ), but the FHiCL document passed to
/// InstanceBuild is tool sub-class-specific. This is as opposed to
/// ConfigureISystProvidersFromParameterHeaders which requires a rigidly
/// structure document.
///
/// The InstanceBuilder function argument is used to instantiate
/// ISystProvider_tool instances held by std::unique_ptrs. When running with
/// ART support this will default to
/// art::make_tool<larsyst::ISystProvider_tool>, but when running outside of
/// art, other instantiators must be used.
provider_list_t ConfigureISystProvidersFromToolConfig(
    fhicl::ParameterSet const &paramset,
    std::function<std::unique_ptr<larsyst::ISystProvider_tool>(
        fhicl::ParameterSet const &)>
        InstanceBuilder
#ifndef NO_ART
    = art::make_tool<larsyst::ISystProvider_tool>
#endif
    ,
    std::string const &key = "syst_providers", paramId_t offset = 0);

///\brief Configures the set of ISystProviders from a Parameter Headers
/// document.
///
/// The structure of paramset must adhere to the Parameter Headers structure
/// described in larsyst/doc/ParameterHeaders.md
///
/// The InstanceBuilder function argument is used to instantiate
/// ISystProvider_tool instances held by std::unique_ptrs. When running with
/// ART support this will default to
/// art::make_tool<larsyst::ISystProvider_tool>, but when running outside of
/// art, other instantiators must be used.
provider_list_t ConfigureISystProvidersFromParameterHeaders(
    fhicl::ParameterSet const &paramset,
    std::function<std::unique_ptr<larsyst::ISystProvider_tool>(
        fhicl::ParameterSet const &)>
        InstanceBuilder
#ifndef NO_ART
    = art::make_tool<larsyst::ISystProvider_tool>
#endif
    ,
    std::string const &key = "syst_providers");

} // namespace larsyst

#endif
