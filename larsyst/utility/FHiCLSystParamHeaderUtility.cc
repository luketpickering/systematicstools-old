#include "larsyst/utility/FHiCLSystParamHeaderUtility.hh"

#include "larsyst/utility/string_parsers.hh"

#include "larsyst/interface/SystMetaData.hh"
#include "larsyst/interface/types.hh"

#include "fhiclcpp/ParameterSet.h"

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

namespace larsyst {

SystParamHeader FHiCLToSystParamHeader(fhicl::ParameterSet const &paramset) {

  static std::vector<std::string> allowed_keys = {"prettyName",
                                                  "systParamId",
                                                  "isWeightSystematicVariation",
                                                  "unitsAreNatural",
                                                  "differsEventByEvent",
                                                  "centralParamValue",
                                                  "isCorrection",
                                                  "oneSigmaShifts",
                                                  "paramValidityRange",
                                                  "isSplineable",
                                                  "isRandomlyThrown",
                                                  "paramVariations",
                                                  "isResponselessParam",
                                                  "responseParamId",
                                                  "responses",
                                                  "opts"};

  for (std::string const &key : paramset.get_names()) {
    if (std::find(allowed_keys.begin(), allowed_keys.end(), key) ==
        allowed_keys.end()) {
      throw invalid_SystParamHeader_key()
          << "[ERROR]: When parsing fhicl::ParameterSet as a "
             "larsyst::SystParamHeader, encountered key "
          << std::quoted(key) << " which was not expected.";
    }
  }

  SystParamHeader sph;
  sph.prettyName = paramset.get<std::string>("prettyName");
  sph.systParamId = paramset.get<paramId_t>("systParamId");
  paramset.get_if_present("isWeightSystematicVariation",
                          sph.isWeightSystematicVariation);
  paramset.get_if_present("unitsAreNatural", sph.unitsAreNatural);
  paramset.get_if_present("differsEventByEvent", sph.differsEventByEvent);
  paramset.get_if_present("centralParamValue", sph.centralParamValue);
  paramset.get_if_present("isCorrection", sph.isCorrection);
  paramset.get_if_present("oneSigmaShifts", sph.oneSigmaShifts);
  paramset.get_if_present("paramValidityRange", sph.paramValidityRange);
  paramset.get_if_present("isSplineable", sph.isSplineable);
  paramset.get_if_present("isRandomlyThrown", sph.isRandomlyThrown);
  paramset.get_if_present("paramVariations", sph.paramVariations);
  paramset.get_if_present("isResponselessParam", sph.isResponselessParam);
  paramset.get_if_present("responseParamId", sph.responseParamId);
  paramset.get_if_present("responses", sph.responses);
  paramset.get_if_present("opts", sph.opts);

  return sph;
}

fhicl::ParameterSet SystParamHeaderToFHiCL(SystParamHeader const &sph) {
  fhicl::ParameterSet ps;

  if (!Validate(sph)) {
    (void)Validate(sph, false);
    std::cout << "[ERROR]: Parameter set (" << sph.systParamId << ":"
              << std::quoted(sph.prettyName) << ") failed validation."
              << std::endl;
    throw;
  }

  ps.put("prettyName", sph.prettyName);
  ps.put("systParamId", sph.systParamId);
  if (!sph.isWeightSystematicVariation) {
    ps.put("isWeightSystematicVariation", sph.isWeightSystematicVariation);
  }
  if (sph.unitsAreNatural) {
    ps.put("unitsAreNatural", sph.unitsAreNatural);
  }
  if (!sph.differsEventByEvent) {
    ps.put("differsEventByEvent", sph.differsEventByEvent);
  }
  if (sph.centralParamValue != kDefaultDouble) {
    ps.put("centralParamValue", sph.centralParamValue);
  }
  if (sph.isCorrection) {
    ps.put("isCorrection", sph.isCorrection);
  }
  if (sph.oneSigmaShifts[0] != kDefaultDouble) {
    ps.put("oneSigmaShifts",
           std::vector<double>{sph.oneSigmaShifts[0], sph.oneSigmaShifts[1]});
  }
  if ((sph.paramValidityRange[0] != kDefaultDouble) ||
      (sph.paramValidityRange[1] != kDefaultDouble)) {
    ps.put("paramValidityRange",
           std::vector<double>{sph.paramValidityRange[0],
                               sph.paramValidityRange[1]});
  }
  if (sph.isSplineable) {
    ps.put("isSplineable", sph.isSplineable);
  }
  if (sph.isRandomlyThrown) {
    ps.put("isRandomlyThrown", sph.isRandomlyThrown);
  }
  if (sph.paramVariations.size()) {
    ps.put("paramVariations", sph.paramVariations);
  }
  if (sph.isResponselessParam) {
    ps.put("isResponselessParam", sph.isResponselessParam);
  }
  if (sph.responseParamId != kParamUnhandled<paramId_t>) {
    ps.put("responseParamId", sph.responseParamId);
  }
  if (sph.responses.size()) {
    ps.put("responses", sph.responses);
  }
  if (sph.opts.size()) {
    ps.put("opts", sph.opts);
  }

  return ps;
}

bool ParseFHiCLVariationDescriptor(fhicl::ParameterSet const &paramset,
                                   std::string const &CV_key,
                                   std::string const &vardescriptor_key,
                                   SystParamHeader &hdr) {
  bool has_cv = paramset.has_key(CV_key);
  bool has_var = paramset.has_key(vardescriptor_key);

  if (!has_cv && !has_var) {
    return false;
  }

  paramset.get_if_present(CV_key, hdr.centralParamValue);
  std::string var_descriptor;
  paramset.get_if_present(vardescriptor_key, var_descriptor);

  trim(var_descriptor);

  if (var_descriptor.size()) {
    char fchar = var_descriptor.front();
    var_descriptor = var_descriptor.substr(1, var_descriptor.length() - 2);
    trim(var_descriptor);
    if (fchar == '(') { // Spline knots
      std::vector<double> range_step_values = ParseToVect<double>(
          var_descriptor.substr(1, var_descriptor.size() - 2), ",");
      if (range_step_values.size() != 3) {
        throw invalid_FHiCL_variation_descriptor()
            << "[ERROR]: When parsing spline knot descriptor found "
            << std::quoted(var_descriptor)
            << ", but the descriptor must be in the format: "
               "(<start>,<end>,<step>).";
      }
      hdr.paramVariations.push_back(range_step_values[0]);
      while ((hdr.paramVariations.back() + range_step_values[2]) <
             range_step_values[1]) {
        hdr.paramVariations.push_back(hdr.paramVariations.back() +
                                      range_step_values[2]);
      }
      hdr.isSplineable = true;
    } else if (fchar == '[') { // Discrete tweaks
      hdr.paramVariations = ParseToVect<double>(var_descriptor, ",");
    } else if (fchar == '{') { // OneSigmaShifts
      std::vector<double> sigShifts = ParseToVect<double>(var_descriptor, ",");
      if (sigShifts.size() == 1) {
        hdr.oneSigmaShifts[0] = -sigShifts.front();
        hdr.oneSigmaShifts[1] = sigShifts.front();
      } else if (sigShifts.size() == 2) {
        hdr.oneSigmaShifts[0] = sigShifts.front();
        hdr.oneSigmaShifts[1] = sigShifts.back();
      } else {
        throw invalid_FHiCL_variation_descriptor()
            << "[ERROR]: When parsing sigma shifts found "
            << std::quoted(var_descriptor)
            << ", but expected {sigma_both_natural_units}, or "
               "{sigma_low_natural_units, sigma_up_natural_units}.";
      }
      hdr.isRandomlyThrown = true;
    } else {
      throw invalid_FHiCL_variation_descriptor()
          << "[ERROR]: Found tweak definition " << std::quoted(var_descriptor)
          << ", but expected to find either, \"{sigma_low_natural_units, "
             "sigma_up_natural_units}\" or \"[spline knot 1, spline knot "
             "2, spline knot 3,...]\"";
    }
  } else { // Just use the central value every time
    hdr.isCorrection = true;
  }
  return true;
}

bool MakeFHiCLDefinedRandomVariations(fhicl::ParameterSet const &paramset,
                                      std::string const &nthrows_key,
                                      SystParamHeader &hdr,
                                      std::string const &distribution_key,
                                      uint64_t seed) {
  if (!hdr.isRandomlyThrown) {
    return false;
  }

  if (!paramset.has_key(nthrows_key)) {
    return false;
  }

  size_t NThrows = paramset.get<size_t>(nthrows_key);

  if (!NThrows) {
    return false;
  }

  hdr.paramVariations.clear();

  std::mt19937_64 generator(
      seed == 0 ? std::chrono::steady_clock::now().time_since_epoch().count()
                : seed);
  std::function<double()> RNJesus;

  // Choose distribution
  if (distribution_key.size() && paramset.has_key(distribution_key)) {
    std::string dist_ident = paramset.get<std::string>(distribution_key);

    if ((dist_ident == "normal") || (dist_ident == "gaussian")) {
      std::normal_distribution<double> distribution(0, 1);
      RNJesus = std::bind(distribution, generator);
    } else if (dist_ident == "uniform") {
      std::uniform_real_distribution<double> distribution(-1, 1);
      RNJesus = std::bind(distribution, generator);
    }
  } else {
    std::normal_distribution<double> distribution(0, 1);
    RNJesus = std::bind(distribution, generator);
  }

  double cv =
      (hdr.centralParamValue == kDefaultDouble) ? 0 : hdr.centralParamValue;
  for (uint64_t t = 0; t < NThrows; ++t) {
    double thr = RNJesus();
    double shift =
        fabs(thr) * ((thr < 0) ? hdr.oneSigmaShifts[0] : hdr.oneSigmaShifts[1]);
    hdr.paramVariations.push_back(cv + shift);
  }

  return true;
}

bool ParseFHiCLSimpleToolConfigurationParameter(
    fhicl::ParameterSet const &paramset, std::string const &parameter_name,
    SystParamHeader &hdr, uint64_t seed) {

  std::string CV_key = parameter_name + "_central_value";
  std::string Tweak_key = parameter_name + "_variation_descriptor";

  if (!ParseFHiCLVariationDescriptor(paramset, CV_key, Tweak_key, hdr)) {
    return false;
  }

  std::string NThrows_key = parameter_name + "_nthrows";
  std::string RandDist_key = parameter_name + "_random_distribution";

  MakeFHiCLDefinedRandomVariations(paramset, NThrows_key, hdr, RandDist_key,
                                   seed);

  return true;
}

} // namespace larsyst
