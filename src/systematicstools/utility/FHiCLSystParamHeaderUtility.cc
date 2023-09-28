#include "systematicstools/utility/FHiCLSystParamHeaderUtility.hh"

#include "systematicstools/utility/string_parsers.hh"

#include "systematicstools/interface/SystMetaData.hh"
#include "systematicstools/interface/types.hh"

#include "fhiclcpp/ParameterSet.h"

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

namespace systtools {

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
    std::string var_descriptor_trimmed =
        var_descriptor.substr(1, var_descriptor.length() - 2);
    trim(var_descriptor_trimmed);
    if (fchar == '(') { // Spline knots
      std::vector<double> range_step_values =
          ParseToVect<double>(var_descriptor_trimmed, ",");
      if (range_step_values.size() != 3) {
        throw invalid_FHiCL_variation_descriptor()
            << "[ERROR]: When parsing spline knot descriptor found "
            << std::quoted(var_descriptor_trimmed)
            << ", but the descriptor must be in the format: "
               "(<start>,<end>,<step>).";
      }
      hdr.paramVariations.push_back(range_step_values[0]);
      while ((hdr.paramVariations.back() + range_step_values[2]) <=
             (range_step_values[1] + std::numeric_limits<double>::epsilon())) {
        hdr.paramVariations.push_back(hdr.paramVariations.back() +
                                      range_step_values[2]);
      }
      hdr.isSplineable = true;
    } else if (fchar == '[') { // Discrete tweaks
      hdr.paramVariations = ParseToVect<double>(var_descriptor_trimmed, ",");
    } else if (fchar == '{') { // OneSigmaShifts
      std::vector<double> sigShifts =
          ParseToVect<double>(var_descriptor_trimmed, ",");
      if (sigShifts.size() == 1) {
        hdr.oneSigmaShifts[0] = -sigShifts.front();
        hdr.oneSigmaShifts[1] = sigShifts.front();
      } else if (sigShifts.size() == 2) {
        hdr.oneSigmaShifts[0] = sigShifts.front();
        hdr.oneSigmaShifts[1] = sigShifts.back();
      } else {
        throw invalid_FHiCL_variation_descriptor()
            << "[ERROR]: When parsing sigma shifts found "
            << std::quoted(var_descriptor_trimmed)
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

    // If there is only one variation, set it as the central value correction
    // instead.
    if (!hdr.isRandomlyThrown) {
      if (hdr.paramVariations.size() == 1) {
        hdr.centralParamValue = hdr.paramVariations.front();
        hdr.paramVariations.clear();
        hdr.isCorrection = true;
      } else if (!hdr.paramVariations.size()) {
        throw invalid_FHiCL_variation_descriptor()
            << "[ERROR]: When parsing " << var_descriptor
            << ", failed to determine any parameter variations.";
      }
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
                                      uint64_t seed, size_t NThrows) {
  if (!hdr.isRandomlyThrown) {
    return false;
  }

  paramset.get_if_present<size_t>(nthrows_key, NThrows);

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

bool FhiclToolConfigurationParameterExists(
    fhicl::ParameterSet const &paramset, std::string const &parameter_name) {

  std::string CV_key = parameter_name + "_central_value";
  std::string Tweak_key = parameter_name + "_variation_descriptor";

  bool has_cv = paramset.has_key(CV_key);
  bool has_var = paramset.has_key(Tweak_key);

  if (has_cv || has_var) {
    return true;
  }
  return false;
}

bool ParseFhiclToolConfigurationParameter(
    fhicl::ParameterSet const &paramset, std::string const &parameter_name,
    SystParamHeader &hdr, uint64_t seed, size_t NThrows) {

  std::string CV_key = parameter_name + "_central_value";
  std::string Tweak_key = parameter_name + "_variation_descriptor";

  if (!ParseFHiCLVariationDescriptor(paramset, CV_key, Tweak_key, hdr)) {
    return false;
  }

  hdr.prettyName = parameter_name;

  std::string NThrows_key = parameter_name + "_nthrows";
  std::string RandDist_key = parameter_name + "_random_distribution";

  MakeFHiCLDefinedRandomVariations(paramset, NThrows_key, hdr, RandDist_key,
                                   seed, NThrows);

  return true;
}

} // namespace systtools
