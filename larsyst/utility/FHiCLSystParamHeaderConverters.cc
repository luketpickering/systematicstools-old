#include "FHiCLSystParamHeaderConverters.hh"

#include "larsyst/interface/SystMetaData.hh"
#include "larsyst/interface/types.hh"

#include "fhiclcpp/ParameterSet.h"

#ifndef NO_ART
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"

#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"
#endif

#include <iomanip>
#include <iostream>
#include <vector>

namespace larsyst {

#ifndef NO_ART
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
                     "calculated when producing a spline or a multi-sim."),
      std::vector<double>{}};

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
#endif

SystParamHeader
FHiCLToSystParamHeader(fhicl::ParameterSet const &paramset) {
#ifndef NO_ART
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
#else // We are using standalone fhiclcpp which doesn't have the
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
#endif

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
  if (sph.centralParamValue != 0xdeadb33f) {
    ps.put("centralParamValue", sph.centralParamValue);
  }
  if (sph.isCorrection) {
    ps.put("isCorrection", sph.isCorrection);
  }
  if (sph.oneSigmaShifts[0] != 0xdeadb33f) {
    ps.put("oneSigmaShifts",
           std::vector<double>{sph.oneSigmaShifts[0], sph.oneSigmaShifts[1]});
  }
  if ((sph.paramValidityRange[0] != 0xdeadb33f) ||
      (sph.paramValidityRange[1] != 0xdeadb33f)) {
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

} // namespace larsyst
