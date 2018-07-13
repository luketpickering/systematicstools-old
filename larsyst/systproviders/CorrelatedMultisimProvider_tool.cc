#include "larsyst/interface/ISystProvider_tool.hh"
#include "larsyst/utility/CovMatThrower.hh"
#include "larsyst/utility/append_event_response.hh"
#include "larsyst/utility/configure_syst_providers.hh"
#include "larsyst/utility/generate_provider_parameter_set.hh"
#include "larsyst/utility/printers.hh"
#include "larsyst/utility/string_parsers.hh"

#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Utilities/ToolMacros.h"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"

#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"

#include "CLHEP/Random/MTwistEngine.h"
#include "CLHEP/Random/RandGaussQ.h"

#include "TFile.h"
#include "TMatrixD.h"

#include <chrono>
#include <sstream>

using namespace larsyst;
using namespace fhicl;

namespace {

struct Config {
  Atom<std::string> inputRootFile{
      Name("inputRootFile"),
      Comment(
          "Path to input root file containing parameter covariance matrix")};
  Atom<std::string> covmatName{
      Name("covmatName"),
      Comment("Name of TMarixDSym to be retrieved from <inputRooTfile>.")};
  Sequence<std::string> parameterPrettyNames{
      Name("parameterPrettyNames"),
      Comment("List of parameter prettyNames to use when interpreting the "
              "rows/columns of the input covariance matrix. Unnamed "
              "parameters will be thrown uncorrelated according to the "
              "normal distribution.")};
  fhicl::Atom<uint64_t> numberOfThrows{
      fhicl::Name("numberOfThrows"),
      fhicl::Comment("Number of correlated throws to make. This will "
                     "override any provided child options.")};
};
} // namespace

class CorrelatedMultisimProvider : public larsyst::ISystProvider_tool {
public:
  explicit CorrelatedMultisimProvider(ParameterSet const &);

  SystMetaData ConfigureFromFHICL(ParameterSet const &, paramId_t);

  bool Configure();
  std::unique_ptr<EventResponse> GetEventResponse(art::Event &);
  std::string AsString();

private:
  std::unique_ptr<CLHEP::HepRandomEngine> RNgine;
  std::unique_ptr<CLHEP::RandGaussQ> RNJesus;

  provider_map_t child_providers;
};

CorrelatedMultisimProvider::CorrelatedMultisimProvider(
    ParameterSet const &params)
    : ISystProvider_tool(params), RNgine{nullptr}, RNJesus{nullptr} {}

std::string CorrelatedMultisimProvider::AsString() {
  return to_str(fMetaData[0]);
}

SystMetaData
CorrelatedMultisimProvider::ConfigureFromFHICL(ParameterSet const &params,
                                               paramId_t firstParamId) {

  std::cout << "[INFO]: Configuring CorrelatedMultisimProvider" << std::endl;

  Table<::Config> cfg{
      params, std::set<std::string>{"tool_type", "uniqueName", "children"}};

  child_providers = larsyst::configure_syst_providers(
      params.get<ParameterSet>("children"), "child_providers", firstParamId);

  std::vector<std::pair<paramId_t, std::vector<double>>> CorrelatedThrows;
  std::vector<std::pair<paramId_t, std::vector<double>>> UncorrelatedThrows;
  std::set<paramId_t> CorrelatedParameters;
  // Check that all specified parameters are handled
  for (auto &pname : cfg().parameterPrettyNames()) {
    std::cout << "[INFO]: Checking that a slave knows about parameter named "
              << std::quoted(pname) << std::endl;
    paramId_t pid = kParamUnhandled<paramId_t>;
    for (auto &sp : child_providers) {
      if (sp.second->ParamIsHandled(pname)) {
        pid = sp.second->GetParameterId(pname);
        std::cout << "\tCurrently handled by " << std::quoted(sp.first)
                  << " as " << pid << std::endl;
        break;
      }
    }
    if (pid == kParamUnhandled<paramId_t>) {
      std::cout
          << "[ERROR]: Could not find a child provider that handles parameter "
          << std::quoted(pname) << std::endl;
      throw;
    } else {
      CorrelatedThrows.push_back(
          std::pair<paramId_t, std::vector<double>>{pid, {}});
      if (CorrelatedParameters.find(pid) != CorrelatedParameters.end()) {
        std::cout
            << "[ERROR]: Already have added " << pid
            << " to the set of known parameters. Is " << pname
            << " specified twice? Parameters with identical prettyNames under "
               "different provider instances cannot be disambiguated."
            << std::endl;
        throw;
      }
      CorrelatedParameters.insert(pid);
    }
  }

  for (auto &sp : child_providers) {
    for (auto &ph : sp.second->GetSystSetConfiguration()) {
      if (CorrelatedParameters.find(ph.systParamId) ==
          CorrelatedParameters.end()) {
        UncorrelatedThrows.push_back(
            std::pair<paramId_t, std::vector<double>>{ph.systParamId, {}});
      }
    }
  }

  TFile *fin = TFile::Open(cfg().inputRootFile().c_str(), "READ");

  if (!fin || !fin->IsOpen()) {
    std::cout << "[ERROR]: Failed to open "
              << std::quoted(cfg().inputRootFile()) << " for reading."
              << std::endl;
    throw;
  }

  TMatrixD *covmat =
      dynamic_cast<TMatrixD *>(fin->Get(cfg().covmatName().c_str()));
  if (!covmat) {
    std::cout << "[ERROR]: Failed to read " << std::quoted(cfg().covmatName())
              << " from " << std::quoted(cfg().inputRootFile()) << std::endl;
    throw;
  }

  RNgine = std::make_unique<CLHEP::MTwistEngine>(fSeedSuggestion);
  RNJesus = std::make_unique<CLHEP::RandGaussQ>(*RNgine);

  CovarianceThrower ct(*covmat, RNgine);

  // Make throws;
  size_t nthrows = cfg().numberOfThrows();
  size_t ncorrparams = CorrelatedThrows.size();
  for (size_t nt_it = 0; nt_it < nthrows; ++nt_it) {
    // Throw correlated vector
    TMatrixD const *thrownVector = ct.Throw();
    for (size_t p_it = 0; p_it < ncorrparams; ++p_it) {
      CorrelatedThrows[p_it].second.push_back((*thrownVector)[p_it][0]);
    }

    // Add uncorrelated throws;
    for (auto &ut : UncorrelatedThrows) {
      ut.second.push_back(RNJesus->fire(0, 1));
    }
  }

  SystMetaData smd;

  // Suggest throws to providers
  for (auto &sp : child_providers) {
    parameter_throws_list_t provider_throws;
    for (auto &ctp : CorrelatedThrows) {
      if (sp.second->ParamIsHandled(ctp.first)) {
        provider_throws.emplace(ctp.first, std::move(ctp.second));
      }
    }
    for (auto &utp : UncorrelatedThrows) {
      if (sp.second->ParamIsHandled(utp.first)) {
        provider_throws.emplace(utp.first, std::move(utp.second));
      }
    }
    sp.second->SuggestParameterThrows(std::move(provider_throws), true);
    for (auto &hdr : sp.second->GetSystSetConfiguration()) {
      hdr.opts.push_back("CorrMSProviderHint=" + sp.second->GetToolType() +
                         "::" + sp.second->GetInstanceName());
      smd.push_back(hdr);
    }
  }
  return smd;
}

bool CorrelatedMultisimProvider::Configure() {
  CheckHaveMetaData();

  std::map<std::pair<std::string, std::string>, std::vector<SystParamHeader>>
      child_syst_provider_parameters;
  for (auto hdr : fMetaData) {
    bool foundToolType = false;

    for (size_t opt_it = 0; opt_it < hdr.opts.size(); ++opt_it) {
      auto const &opt = hdr.opts[opt_it];
      if (opt.find("CorrMSProviderHint=") == 0) {
        auto const &splitHint = ParseToVect<std::string>(opt, "=");
        if (splitHint.size() != 2) {
          std::cout
              << "[ERROR]: CorrelatedMultisimProvider found a parameter "
                 "provider hint: "
              << std::quoted(opt) << ", expected to find one of the form: "
              << std::quoted(
                     "CorrMSProviderHint=<tool_type>::<optional unique_name>")
              << std::endl;
          throw;
        }
        auto const &nameHint =
            ParseToVect<std::string>(splitHint.back(), "::", true);
        if (nameHint.size() != 2) {
          std::cout
              << "[ERROR]: CorrelatedMultisimProvider found a parameter "
                 "provider hint: "
              << std::quoted(opt) << ", expected to find one of the form: "
              << std::quoted(
                     "CorrMSProviderHint=<tool_type>::<optional unique_name>")
              << std::endl;
          throw;
        }
        // Remove the name hint option before passing it onwards.
        std::vector<std::string>::iterator del_it = hdr.opts.begin();
        std::advance(del_it, opt_it);
        hdr.opts.erase(del_it);
        child_syst_provider_parameters[{nameHint.front(), nameHint.back()}]
            .push_back(hdr);
        foundToolType = true;
        break;
      }
    }
    if (!foundToolType) {
      std::cout << "[ERROR]: Failed to find tool_type on child parameter named "
                << hdr.prettyName << ", expected to find one of the form: "
                << std::quoted("CorrMSProviderHint=<tool_type>::<optional "
                               "unique_name>. \n[INFO]: Found opts:")
                << std::endl;
      for (size_t opt_it = 0; opt_it < hdr.opts.size(); ++opt_it) {
        std::cout << "\t" << std::quoted(hdr.opts[opt_it]) << std::endl;
      }
    }
  }

  fhicl::ParameterSet syst_provider_config;

  std::vector<std::string> providerNames;
  for (auto &cph : child_syst_provider_parameters) {
    std::string provider_FQName = cph.first.first;
    if (cph.first.second.size()) {
      provider_FQName += "_" + cph.first.second;
    }
    syst_provider_config.put(provider_FQName,
                             generate_provider_parameter_set(cph));
    providerNames.push_back(provider_FQName);
  }
  syst_provider_config.put("syst_providers", providerNames);

  child_providers =
      larsyst::load_syst_provider_configuration(syst_provider_config);

  return true;
}

std::unique_ptr<EventResponse>
CorrelatedMultisimProvider::GetEventResponse(art::Event &e) {

  std::unique_ptr<EventResponse> er = std::make_unique<EventResponse>();
  bool first = true;
  for (auto &sp : child_providers) {
    std::unique_ptr<larsyst::EventResponse> syst_resp =
        sp.second->GetEventResponse(e);
    if (!syst_resp) {
      std::cout << "[ERROR]: Got null syst response from provider "
                << sp.second->GetFullyQualifiedName() << std::endl;
      throw;
    }
    if (first) {
      er = std::move(syst_resp);
      first = false;
    } else {
      append_event_response(er, std::move(syst_resp));
    }
  }
  return er;
}

DEFINE_ART_CLASS_TOOL(CorrelatedMultisimProvider)
