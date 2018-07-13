#include "build_parameter_set_from_header.hh"

#include "larsyst/interface/SystMetaData.hh"
#include "larsyst/interface/types.hh"

#include "fhiclcpp/ParameterSet.h"

#include <iomanip>
#include <iostream>
#include <vector>

namespace larsyst {

fhicl::ParameterSet
build_parameter_set_from_header(SystParamHeader const &sph) {
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
