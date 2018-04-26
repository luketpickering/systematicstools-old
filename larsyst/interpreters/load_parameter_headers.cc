#include "load_parameter_headers.hh"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"

#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

namespace larsyst {

namespace {
struct FHICLSystMetaData {
  fhicl::Atom<std::string> prettyName{
      fhicl::Name("prettyName"),
      fhicl::Comment("Human readable systematic parameter name")};

  fhicl::Atom<paramId_t> systParamId{
      fhicl::Name("systParamId"),
      fhicl::Comment("Unique identifier for this systematic "
                     "parameter in this param set.")};

  fhicl::Atom<bool> isWeightSystematicVariation{
      fhicl::Name("isWeightSystematicVariation"),
      fhicl::Comment(
          "Whether this systematic corresponds to a weight or some observable "
          "variation. {Default == true}"),
      true};

  fhicl::Atom<bool> unitsAreNatural{
      fhicl::Name("unitsAreNatural"),
      fhicl::Comment("Whether the quantities stored in `paramVariations` and "
                     "`centralParamValue` are in 'natural' units as opposed to "
                     "units of sigma. {Default == false}"),
      false};

  fhicl::Atom<bool> differsEventByEvent{
      fhicl::Name("differsEventByEvent"),
      fhicl::Comment(
          "Whether the the response of this parameter is fully described by "
          "this meta-data for all events that it affects. {Default == true}"),
      true};

  fhicl::Atom<double> centralParamValue{
      fhicl::Name("centralParamValue"),
      fhicl::Comment(
          "The central parameter value used in this systematic evaluation. "
          "{Default == 0xdeadb33f}"),
      0xdeadb33f};

  fhicl::Atom<bool> isCorrection{
      fhicl::Name("isCorrection"),
      fhicl::Comment("Whether this parameter response is considered an "
                     "uncertainty or a single valued parameter 'correction' -- "
                     "i.e. calibration constant, or interaction model shift "
                     "that should always be applied to every relevant event."),
      false};

  fhicl::Sequence<double, 2> oneSigmaShifts{
      fhicl::Name("oneSigmaShifts"),
      fhicl::Comment("The 'one sigma' shifts of this parameter, defined in "
                     "nautral units. {Default == {0xdeadb33f,0xdeadb33f}}"),
      std::array<double, 2>{0xdeadb33f, 0xdeadb33f}};

  fhicl::Sequence<double, 2> paramValidityRange{
      fhicl::Name("paramValidityRange"),
      fhicl::Comment(
          "The validity range of the parameter.'0xdeadb33f' denotes unbounded "
          "on that side. {Default == {0xdeadb33f,0xdeadb33f}}"),
      std::array<double, 2>{0xdeadb33f, 0xdeadb33f}};

  fhicl::Atom<bool> isSplineable{
      fhicl::Name("isSplineable"),
      fhicl::Comment(
          "Whether the `paramVariations` were chosen to facilitate a "
          "downstream consumer to spline the parameter response."),
      false};
  fhicl::Atom<bool> isRandomlyThrown{
      fhicl::Name("isRandomlyThrown"),
      fhicl::Comment("Whether the non-splineable `paramVariations` were "
                     "randomly chosen or hand-picked"),
      false};

  fhicl::Sequence<double> paramVariations{
      fhicl::Name("paramVariations"),
      fhicl::Comment("The shifted values of this parameter that should be "
                     "calculated when producing a spline or a multi-sim.")};

  fhicl::Atom<bool> isResponselessParam{
      fhicl::Name("isResponselessParam"),
      fhicl::Comment("Whether variations of this parameter produce responses "
                     "via this parameter."),
      false};

  fhicl::Atom<paramId_t> responseParamId{
      fhicl::Name("responseParamId"),
      fhicl::Comment("The parameter Id of where responses to parameters with "
                     "isResponselessParam == true can be found."),
      kParamUnhandled<paramId_t>};

  fhicl::Sequence<double> responses{
      fhicl::Name("responses"),
      fhicl::Comment("The parameter responses for 'parameter-level' "
                     "systematics. For most parameters, which effect each "
                     "event differently, this will be empty. {Default == {}}"),
      std::vector<double>{}};

  fhicl::Sequence<std::string> opts{
      fhicl::Name("opts"),
      fhicl::Comment("Arbitrary string options stored in the meta-data for "
                     "further syst-provider configuration. {Default == {}}"),
      std::vector<std::string>{}};
};
} // namespace

SystParamHeader
build_header_from_parameter_set(fhicl::ParameterSet const &paramset) {
  fhicl::Table<FHICLSystMetaData> const result{paramset};

  SystParamHeader sph;
  sph.prettyName = result().prettyName();
  sph.systParamId = result().systParamId();
  sph.isWeightSystematicVariation = result().isWeightSystematicVariation();
  sph.unitsAreNatural = result().unitsAreNatural();
  sph.differsEventByEvent = result().differsEventByEvent();
  sph.centralParamValue = result().centralParamValue();
  sph.isCorrection = result().isCorrection();
  sph.oneSigmaShifts = result().oneSigmaShifts();
  sph.paramValidityRange = result().paramValidityRange();
  sph.isSplineable = result().isSplineable();
  sph.isRandomlyThrown = result().isRandomlyThrown();
  sph.paramVariations = result().paramVariations();
  sph.isResponselessParam = result().isResponselessParam();
  sph.responseParamId = result().responseParamId();
  sph.responses = result().responses();
  sph.opts = result().opts();

  return sph;
}

param_header_map_t
load_syst_provider_headers(fhicl::ParameterSet const &paramset,
                           std::string const &key) {

  param_header_map_t loaded_headers;

  std::cout << "[INFO]: Loading configured syst providers:" << std::endl;
  auto const &providers = paramset.get<std::vector<std::string>>(key);
  for (auto const &provider_name : providers) {
    // Get fhicl config for provider
    std::cout << "[INFO]:\t Retrieving meta data for: \"" << provider_name
              << "\"..." << std::endl;
    auto const &provider_cfg = paramset.get<fhicl::ParameterSet>(provider_name);
    std::cout << " found!" << std::endl;

    std::vector<std::string> const &paramHeadersToRead =
        provider_cfg.get<std::vector<std::string>>("parameterHeaders");

    for (auto const &ph : paramHeadersToRead) {
      SystParamHeader hdr = build_header_from_parameter_set(
          provider_cfg.get<fhicl::ParameterSet>(ph));

      // check that this unique name hasn't been used before.
      if (loaded_headers.find(hdr.systParamId) != loaded_headers.end()) {
        std::cout << "[ERROR]:\t Header describing parameter "
                  << hdr.systParamId << " already exists (provider: "
                  << std::quoted(loaded_headers[hdr.systParamId].first)
                  << ", prettyName: "
                  << std::quoted(
                         loaded_headers[hdr.systParamId].second.prettyName)
                  << ")"
                  << ", this parameter: { provider: " << std::quoted(ph)
                  << ", prettyName: " << std::quoted(hdr.prettyName)
                  << "} cannot be added. " << std::endl;
        throw;
      }

      loaded_headers.emplace(param_header_map_t::key_type{hdr.systParamId},
                             param_header_map_t::mapped_type{ph, hdr});
    }
  }
  std::cout << "[INFO]: Loaded " << loaded_headers.size()
            << " systematic parameter headers from " << providers.size()
            << " providers." << std::endl;

  return loaded_headers;
}
} // namespace larsyst
