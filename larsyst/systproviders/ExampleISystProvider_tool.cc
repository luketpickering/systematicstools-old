#include "larsyst/systproviders/ExampleISystProvider_tool.hh"

#include "larsyst/utility/printers.hh"
#include "larsyst/utility/string_parsers.hh"

#ifndef NO_ART
#include "art/Utilities/ToolMacros.h"
#endif

using namespace larsyst;
using namespace fhicl;

// 1 sigma = 5% normalisation uncertainty by default.
double default_centralvalue_nu = 1;
double default_lowsigmavalue_nu = 5;
double default_upsigmavalue_nu = 5;

ExampleISystProvider::ExampleISystProvider(ParameterSet const &params)
    : ISystProvider_tool(params), RNgine{nullptr}, RNJesus{nullptr} {}

double GetNormResponse(double param_val_nu) { return 1 + param_val_nu * 0.01; }

double GetLateralResponse(double param_val_nu) { return param_val_nu; }

double GetResponse_nu(double param_val_nu, SystParamHeader const &sph) {
  return sph.isWeightSystematicVariation ? GetNormResponse(param_val_nu)
                                         : GetLateralResponse(param_val_nu);
}

double GetParamShift_nu(double shift_sigma, SystParamHeader const &sph) {
  return fabs(shift_sigma) *
         (shift_sigma < 0 ? sph.oneSigmaShifts[0] : sph.oneSigmaShifts[1]);
}

double GetParamValue_nu(double shift_sigma, SystParamHeader const &sph) {
  if (sph.unitsAreNatural) {
    return sph.centralParamValue + GetParamShift_nu(shift_sigma, sph);
  } else {
    return ::default_centralvalue_nu + GetParamShift_nu(shift_sigma, sph);
  }
}

double GetResponse_shift(double shift_sigma, SystParamHeader const &sph) {
  return GetResponse_nu(GetParamValue_nu(shift_sigma, sph), sph);
}

double GetResponse(double val, SystParamHeader const &sph) {
  return sph.unitsAreNatural ? GetResponse_nu(val, sph)
                             : GetResponse_shift(val, sph);
}

std::string ExampleISystProvider::AsString() {
  CheckHaveMetaData();
  return to_str(GetSystMetaData().front());
}

SystMetaData ExampleISystProvider::BuildSystMetaData(ParameterSet const &params,
                                                     paramId_t firstParamId) {

  std::cout << "[INFO]: Configuring ExampleISystProvider" << std::endl;

  SystParamHeader sph;

  sph.systParamId = firstParamId;

  bool isLateral = false;
  params.get_if_present("provide_lateral", isLateral);
  sph.isWeightSystematicVariation = !isLateral;
  bool isGlobal = false;
  params.get_if_present("is_global", isGlobal);
  sph.differsEventByEvent = !isGlobal;
  params.get_if_present("use_natural_units", sph.unitsAreNatural);

  sph.centralParamValue = ::default_centralvalue_nu;

  params.get_if_present("apply_to_all", applyToAll);

  if (params.has_key("param_name")) {
    sph.prettyName = params.get<std::string>("param_name");
  } else {
    sph.prettyName = "ExampleLArSyst";
    sph.prettyName += std::string(isGlobal ? "Global" : "EventByEvent") +
                      std::string(isLateral ? "Norm" : "Lateral") +
                      std::string(applyToAll ? "_all" : "_some");
  }

  if (!ParseFHiCLVariationDescriptor(params, "central_value",
                                     "variation_descriptor", sph)) {

    if (params.has_key("number_of_throws")) {
      sph.isRandomlyThrown = true;
    } else {
      throw invalid_ToolConfigurationFHiCL()
          << "[ERROR]: Tool configuration: { " << params.to_indented_string()
          << " } did not contain enough information to configure. See "
             "larsyst/systproviders/ExampleISystProvider_tool.hh for minimal "
             "configuration.";
    }
  }

  MakeFHiCLDefinedRandomVariations(params, "number_of_throws", sph, "rand_dist",
                                   fSeedSuggestion);

  if (!sph.differsEventByEvent) {
    // Need to add global responses
    for (double var : sph.paramVariations) {
      sph.responses.push_back(GetResponse(var, sph));
    }
  }

  return SystMetaData{{sph}};
}

ParameterSet ExampleISystProvider::GetExtraToolOptions() {
  ParameterSet options;
  options.put("apply_to_all", applyToAll);
  return options;
}

bool ExampleISystProvider::SetupResponseCalculator(
    ParameterSet const &options) {

  CheckHaveMetaData();

  applyToAll = options.get<bool>("apply_to_all");

  if (!applyToAll && !RNJesus) {
    RNgine = std::make_unique<std::mt19937_64>(0);
    RNJesus = std::make_unique<std::normal_distribution<double>>(0, 1);
  }

  return true;
}

#ifndef NO_ART
std::unique_ptr<EventResponse>
ExampleISystProvider::GetEventResponse(art::Event &e) {
  if (!applyToAll) {
    // If we aren't applying it to all events, randomly select events to give
    // weights to.
    if (RNJesus(RNgine) > 0) {
      std::unique_ptr<EventResponse> er = std::make_unique<EventResponse>();
      er->responses.push_back(std::map<uint, std::vector<double>>{});
      return er;
    }
  }

  std::unique_ptr<EventResponse> er = std::make_unique<EventResponse>();
  if (!GetSystMetaData().front().differsEventByEvent) {
    er->responses.push_back(std::map<uint, std::vector<double>>{
        {GetSystMetaData().front().systParamId, std::vector<double>{}}});
  } else {
    std::vector<double> responses;
    for (auto psv : GetSystMetaData().front().paramVariations) {
      responses.push_back(GetResponse(psv, GetSystMetaData().front()));
    }
    er->responses.push_back(std::map<uint, std::vector<double>>{
        {GetSystMetaData().front().systParamId, responses}});
  }
  return er;
}

DEFINE_ART_CLASS_TOOL(ExampleISystProvider)
#endif
