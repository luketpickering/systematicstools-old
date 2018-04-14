// NuInteractSyst_CoulombCorrection.cxx
//
// Implements a simple coulomb field correction for charged-current neutrino
// interactions
//
// Written by picker24 [at] msu [dot] edu

#include "larsyst/interface/ISystProvider.hxx"

using namespace larsyst;

namespace {
int kMuon = 13;
int kAntiMuon = -13;
// int kElectron = 11;
// int kAntiElectron = -11;
} // namespace

class NuInteractSyst_CoulombCorrection : public larsyst::ISystProvider {
public:
  NuInteractSyst_CoulombCorrection()
      : ISystProvider("NuInteractSyst_CoulombCorrection"){};
  char Configure(fhicl::ParameterSet const &, char);
  SystMetaData GetSystMetaData() const;
  EventResponse GetEventResponse(art::Event &);

private:
  std::map<int, SystParamHeader> md;

  DECLARE_ISYSTPROVIDER(NuInteractSyst_CoulombCorrection)
};

REGISTER_ISYSTPROVIDER(NuInteractSyst_CoulombCorrection)

char NuInteractSyst_CoulombCorrection::Configure(
    fhicl::ParameterSet const &params, char systId) {
  std::cout << "[INFO]: Configuring NuInteractSyst_CoulombCorrection"
            << std::endl;

  md[kMuon].prettyName = "NuInteractSyst_CoulombCorrection_NormUncert_muon";
  md[kMuon].systParamId = systId++;
  md[kMuon].isWeightSystematicVariation = true;
  md[kMuon].unitsAreNatural = false;
  md[kMuon].differsEventByEvent = false;
  md[kMuon].centralParamValue = 1;
  md[kMuon].oneSigmaShifts = {2, 2};
  md[kMuon].multiDimDescriptor = {};
  md[kMuon].isSplineable = true;
  md[kMuon].paramShiftValues = {-2, -1, 0, 1, 2};
  for (auto psv : md[kMuon].paramShiftValues) {
    bool shiftIsNeg = psv < 0;
    md[kMuon].responses.push_back(abs(psv) *
                                  (shiftIsNeg ? md[kMuon].oneSigmaShifts[0]
                                              : md[kMuon].oneSigmaShifts[1]));
  }

  md[kAntiMuon].prettyName =
      "NuInteractSyst_CoulombCorrection_NormUncert_antimuon";
  md[kAntiMuon].systParamId = systId++;
  md[kAntiMuon].isWeightSystematicVariation = true;
  md[kAntiMuon].unitsAreNatural = false;
  md[kAntiMuon].differsEventByEvent = false;
  md[kAntiMuon].centralParamValue = 1;
  md[kAntiMuon].oneSigmaShifts = {1, -1};
  md[kAntiMuon].multiDimDescriptor = {};
  md[kAntiMuon].isSplineable = true;
  md[kAntiMuon].paramShiftValues = {-2, -1, 0, 1, 2};
  for (auto psv : md[kAntiMuon].paramShiftValues) {
    bool shiftIsNeg = psv < 0;
    md[kAntiMuon].responses.push_back(
        abs(psv) * (shiftIsNeg ? md[kAntiMuon].oneSigmaShifts[0]
                               : md[kAntiMuon].oneSigmaShifts[1]));
  }

  return systId;
}

SystMetaData NuInteractSyst_CoulombCorrection::GetSystMetaData() const {

  SystMetaData allmd;
  for (auto &p : md) {
    allmd.headers.emplace_back(std::move(p.second));
  }

  return allmd;
}

EventResponse NuInteractSyst_CoulombCorrection::GetEventResponse(art::Event &) {
  return EventResponse{};
}
