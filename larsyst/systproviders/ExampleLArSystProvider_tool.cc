#include "larsyst/interface/ISystProvider_tool.hh"
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

#include <chrono>
#include <sstream>

using namespace larsyst;
using namespace fhicl;

namespace {

// 1 sigma = 5% normalisation uncertainty by default.
double default_centralvalue_nu = 1;
double default_lowsigmavalue_nu = 5;
double default_upsigmavalue_nu = 5;

struct Config {
  Atom<bool> unitsAreNatural{
      Name("unitsAreNatural"),
      Comment("Whether paramVariations should be produced in natural units "
              "(via oneSigmaShifts) or in units of sigma. {Default == false}"),
      false};
  Atom<double> centralParamValue{
      Name("centralParamValue"),
      Comment("Use to override the default central parameter value. Can only "
              "specified in nautral units and for multi-sim parameters."),
      [this]() -> bool {
        return this->unitsAreNatural() && !this->splineKnotDescriptor().size();
      },
      default_centralvalue_nu};
  Sequence<double, 2> oneSigmaShifts{
      Name("oneSigmaShifts"),
      Comment(
          "Use to override the width of multi-sim throws, in nautral units."),
      [this]() -> bool { return !this->splineKnotDescriptor().size(); },
      {default_lowsigmavalue_nu, default_upsigmavalue_nu}};

  Atom<std::string> splineKnotDescriptor{
      Name("splineKnotDescriptor"), Comment(""),
      [this]() -> bool {
        return (this->numberOfThrows() == 0) &&
               (this->centralParamValue() == 0);
      },
      ""};

  Atom<uint64_t> numberOfThrows{
      Name("numberOfThrows"),
      Comment("Number of throws if not using splineKnotDescriptor."),
      [this]() -> bool { return !this->splineKnotDescriptor().size(); }, 0};

  Atom<bool> provideLateral{
      Name("provideLateral"),
      Comment("Apply a lateral systematic shift, as opposed to a weight."),
      false};

  Atom<std::string> paramName{Name("paramName"),
                              Comment("Optional parameter name override"), ""};

  Atom<bool> isGlobal{Name("isGlobal"),
                      Comment("Whether throws are added to the event record or "
                              "fully described by the header."),
                      true};

  Atom<bool> applyToAll{Name("applyToAll"),
                        Comment("Apply to all events? If not, randomly applies "
                                "to half of the events. {Default == false}"),
                        false};
};
} // namespace

class ExampleLArSystProvider : public larsyst::ISystProvider_tool {
public:
  explicit ExampleLArSystProvider(ParameterSet const &);

  SystMetaData ConfigureFromFHICL(ParameterSet const &, paramId_t);

  bool Configure();
  std::unique_ptr<EventResponse> GetEventResponse(art::Event &);
  std::string AsString();

  void SuggestParameterThrows(parameter_throws_list_t &&throws, bool Check);

private:
  bool applyToAll;
  std::unique_ptr<CLHEP::HepRandomEngine> RNgine;
  std::unique_ptr<CLHEP::RandGaussQ> RNJesus;
};

ExampleLArSystProvider::ExampleLArSystProvider(ParameterSet const &params)
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

std::string ExampleLArSystProvider::AsString() {
  CheckHaveMetaData();
  return to_str(fMetaData.headers[0]);
}

SystMetaData
ExampleLArSystProvider::ConfigureFromFHICL(ParameterSet const &params,
                                           paramId_t firstParamId) {

  std::cout << "[INFO]: Configuring ExampleLArSystProvider" << std::endl;

  Table<::Config> cfg{params, std::set<std::string>{"tool_type", "uniqueName"}};

  SystParamHeader sph;

  if (cfg().paramName().size()) {
    sph.prettyName = cfg().paramName();
  } else {
    sph.prettyName = "ExampleLArSyst";
    sph.prettyName +=
        std::string(cfg().isGlobal() ? "Global" : "EventByEvent") +
        std::string(cfg().provideLateral() ? "Norm" : "Lateral") +
        std::string(cfg().applyToAll() ? "_all" : "_some");
  }
  sph.systParamId = firstParamId;

  sph.isWeightSystematicVariation = !cfg().provideLateral();
  sph.differsEventByEvent = !cfg().isGlobal();
  sph.unitsAreNatural = cfg().unitsAreNatural();

  sph.centralParamValue = cfg().centralParamValue();
  sph.oneSigmaShifts = cfg().oneSigmaShifts();
  std::string knot_descriptor = cfg().splineKnotDescriptor();
  uint64_t NThrows = cfg().numberOfThrows();

  if (cfg().applyToAll()) {
    sph.opts.push_back("all");
  }

  if (knot_descriptor.size()) {
    sph.paramVariations = ParseDoubleListDescriptor(knot_descriptor);

    if (!sph.differsEventByEvent) {
      for (size_t k = 0; k < sph.paramVariations.size(); ++k) {
        sph.responses.push_back(GetResponse(sph.paramVariations[k], sph));
      }
    }

    sph.isSplineable = true;
  } else if (NThrows) {
    uint64_t rngSeed = 0;
    if (!fSeedSuggestion) {
      rngSeed = std::chrono::steady_clock::now().time_since_epoch().count();
    } else if (fSeedSuggestion) {
      rngSeed = fSeedSuggestion;
    }
    RNgine = std::make_unique<CLHEP::MTwistEngine>(rngSeed);
    RNJesus = std::make_unique<CLHEP::RandGaussQ>(*RNgine);

    for (size_t t = 0; t < NThrows; ++t) {
      double norm_throw = RNJesus->fire(0, 1);
      sph.paramVariations.push_back(
          sph.unitsAreNatural ? GetParamValue_nu(norm_throw, sph) : norm_throw);
      if (!sph.differsEventByEvent) {
        sph.responses.push_back(GetResponse(sph.paramVariations.back(), sph));
      }
    }
    sph.isSplineable = false;
    sph.isRandomlyThrown = true;
  } else {
    std::cout << "[ERROR]: Bad configuration, expected to see one of "
                 "splineKnotDescriptor, or numberOfThrows."
              << std::endl;
    cfg.print_allowed_configuration(std::cout);
    throw;
  }

  std::cout << "[INFO]: Configured systematic provider:\n\t" << to_str(sph)
            << std::endl;

  return {{sph}};
}

void ExampleLArSystProvider::SuggestParameterThrows(
    parameter_throws_list_t &&throws, bool Check) {

  CheckHaveMetaData();

  if (Check) {
    for (auto &p : throws) {
      SystParamHeader const &sph = Header(p.first);
      if (!sph.isRandomlyThrown) {
        std::cout
            << "[ERROR]: SuggestParameterThrows Check failed. Parameter with "
               "id \""
            << p.first << "\", name: \"" << sph.prettyName
            << "\" is not a random parameter." << std::endl;
        throw;
      }
    }
  }

  for (auto &&p : throws) {
    size_t idx = GetParameterHeaderMetaDataIndex(p.first);
    SystParamHeader &hdr = fMetaData.headers[idx];
    hdr.paramVariations = std::move(p.second);

    if (hdr.unitsAreNatural) {
      for (auto &psv : hdr.paramVariations) {
        psv += hdr.centralParamValue;
      }
    }

    hdr.responses.clear();

    if (!hdr.differsEventByEvent) {
      // Need to calculate responses to throws here
      for (size_t it = 0; it < hdr.paramVariations.size(); ++it) {
        hdr.responses.push_back(GetResponse(hdr.paramVariations[it], hdr));
      }
    }
  }
}

bool ExampleLArSystProvider::Configure() {

  CheckHaveMetaData();

  if (std::find(fMetaData.headers[0].opts.begin(),
                fMetaData.headers[0].opts.end(),
                "all") != fMetaData.headers[0].opts.end()) {
    applyToAll = true;
  } else {
    if (!RNJesus) {
      RNgine = std::make_unique<CLHEP::MTwistEngine>(0);
      RNJesus = std::make_unique<CLHEP::RandGaussQ>(*RNgine);
    }
  }

  return true;
}

std::unique_ptr<EventResponse>
ExampleLArSystProvider::GetEventResponse(art::Event &e) {
  if (!applyToAll) {
    if (RNJesus->fire(0, 1) > 0) {
      std::unique_ptr<EventResponse> er = std::make_unique<EventResponse>();
      er->responses.push_back(std::map<uint, std::vector<double>>{});
      return er;
    }
  }

  std::unique_ptr<EventResponse> er = std::make_unique<EventResponse>();
  if (!fMetaData.headers[0].differsEventByEvent) {
    er->responses.push_back(std::map<uint, std::vector<double>>{
        {fMetaData.headers[0].systParamId, std::vector<double>{}}});
  } else {
    std::vector<double> responses;
    for (auto psv : fMetaData.headers[0].paramVariations) {
      responses.push_back(GetResponse(psv, fMetaData.headers[0]));
    }
    er->responses.push_back(std::map<uint, std::vector<double>>{
        {fMetaData.headers[0].systParamId, responses}});
  }
  return er;
}

DEFINE_ART_CLASS_TOOL(ExampleLArSystProvider)
