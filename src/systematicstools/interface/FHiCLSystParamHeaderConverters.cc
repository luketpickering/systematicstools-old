#include "systematicstools/interface/FHiCLSystParamHeaderConverters.hh"

#include "systematicstools/utility/string_parsers.hh"

#include "systematicstools/interface/types.hh"

#include "fhiclcppsimple/ParameterSet.h"

#include <vector>
#include <iomanip>

namespace systtools {

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
             "systtools::SystParamHeader, encountered key "
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
    throw invalid_SystParamHeader()
        << "[ERROR]: Parameter set (" << sph.systParamId << ":"
        << std::quoted(sph.prettyName) << ") failed validation.";
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

} // namespace systtools
