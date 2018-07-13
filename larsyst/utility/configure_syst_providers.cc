#include "configure_syst_providers.hh"

#include "art/Utilities/make_tool.h"

#include "CLHEP/Random/MTwistEngine.h"
#include "CLHEP/Random/RandFlat.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <vector>

namespace larsyst {

provider_map_t configure_syst_providers(fhicl::ParameterSet const &paramset,
                                        std::string const &key,
                                        paramId_t offset) {

  std::unique_ptr<CLHEP::HepRandomEngine> RNgine =
      std::make_unique<CLHEP::MTwistEngine>(
          std::chrono::steady_clock::now().time_since_epoch().count());
  std::unique_ptr<CLHEP::RandFlat> seedSuggester =
      std::make_unique<CLHEP::RandFlat>(*RNgine);

  provider_map_t loaded_providers;

  std::cout << "[INFO]: Loading specified syst providers:" << std::endl;
  paramId_t syst_param_id = offset;
  for (auto const &provider_name :
       paramset.get<std::vector<std::string>>(key)) {
    // Get fhicl config for provider
    std::cout << "[INFO]:\t Retrieving configuration for: \"" << provider_name
              << "\"..." << std::endl;
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provider_name);
    std::cout << " found!" << std::endl;

    // make an instance of the plugin
    std::cout << "[INFO]:\t Requesting provider instance..." << std::endl;
    std::unique_ptr<larsyst::ISystProvider_tool> is =
        art::make_tool<larsyst::ISystProvider_tool>(provider_cfg);
    std::cout << " success!" << std::endl;

    // configure the plugin
    std::cout << "[INFO]:\t Configuring provider instance..." << std::endl;

    is->SuggestSeed(seedSuggester->fireInt(1E6));
    SystMetaData md =
        is->GenerateSystSetConfiguration(provider_cfg, syst_param_id);
    std::cout << "[INFO]\t Success!" << std::endl;
    syst_param_id += md.size();

    // build unique name
    std::string FQName = is->GetFullyQualifiedName();
    std::cout
        << "[INFO]:\t Attempting to register provider with fully qualifed "
           "name: \""
        << FQName << "\"..." << std::endl;

    // check that this unique name hasn't been used before.
    if (loaded_providers.find(FQName) != loaded_providers.end()) {
      std::cout << "failed." << std::endl
                << "[ERROR]:\t Provider with that name already exists, please "
                   "correct provider set (Hint: Use the 'unique_name' property "
                   "of the tool configuration table to dismabiguate multiple "
                   "uses of the same tool)."
                << std::endl;
      throw;
    }
    std::cout << "Success!" << std::endl;
    // std::cout << "[INFO]: Configured " << is->AsString() << std::endl;
    loaded_providers.emplace(std::move(FQName), std::move(is));
  }
  std::cout << "[INFO]: Configured " << loaded_providers.size()
            << " systematic providers with " << (syst_param_id - offset)
            << " parameters." << std::endl;
  return loaded_providers;
}

provider_map_t
load_syst_provider_configuration(fhicl::ParameterSet const &paramset,
                                 std::string const &key) {

  provider_map_t loaded_providers;

  size_t nparams = 0;
  std::cout << "[INFO]: Loading configured syst providers:" << std::endl;
  for (auto const &provider_name :
       paramset.get<std::vector<std::string>>(key)) {
    // Get fhicl config for provider
    std::cout << "[INFO]:\t Retrieving meta data for: \"" << provider_name
              << "\"..." << std::endl;
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provider_name);
    std::cout << " found!" << std::endl;

    // make an instance of the plugin
    std::cout << "[INFO]:\t Requesting provider instance..." << std::endl;
    std::unique_ptr<larsyst::ISystProvider_tool> is =
        art::make_tool<larsyst::ISystProvider_tool>(provider_cfg);
    std::cout << " success!" << std::endl;

    // configure the plugin
    std::cout << "[INFO]:\t Loading provider configuration..." << std::endl;

    if (is->ReadParameterHeaders(provider_cfg)) {
      std::cout << "[INFO]\t Success!" << std::endl;
    } else {
      std::cout << "[ERROR]:\t Failure." << std ::endl;
      throw;
    }
    nparams += is->GetSystSetConfiguration().size();

    // build unique name
    std::string FQName = is->GetFullyQualifiedName();
    std::cout
        << "[INFO]:\t Attempting to register provider with fully qualifed "
           "name: \""
        << FQName << "\"..." << std::endl;

    // check that this unique name hasn't been used before.
    if (loaded_providers.find(FQName) != loaded_providers.end()) {
      std::cout << "failed." << std::endl
                << "[ERROR]:\t Provider with that name already exists, please "
                   "correct provider set (Hint: Use the 'unique_name' property "
                   "of the tool configuration table to dismabiguate multiple "
                   "uses of the same tool)."
                << std::endl;
      throw;
    }
    std::cout << "Success!" << std::endl;
    // std::cout << "[INFO]: Configured " << is->AsString() << std::endl;
    loaded_providers.emplace(std::move(FQName), std::move(is));
  }
  std::cout << "[INFO]: Loaded " << loaded_providers.size()
            << " systematic providers with " << nparams << " parameters."
            << std::endl;
  return loaded_providers;
}

} // namespace larsyst
