#ifndef SYSTTOOLS_SYSTPROVIDERS_EXAMPLEISYSTPROVIDER_SEEN
#define SYSTTOOLS_SYSTPROVIDERS_EXAMPLEISYSTPROVIDER_SEEN

#include "systematicstools/interface/ISystProviderTool.hh"

#include <memory>
#include <random>
#include <string>

class ExampleISystProvider : public systtools::ISystProviderTool {
public:
  explicit ExampleISystProvider(fhicl::ParameterSet const &);

  systtools::SystMetaData BuildSystMetaData(fhicl::ParameterSet const &,
                                          systtools::paramId_t);
  fhicl::ParameterSet GetExtraToolOptions();
  bool SetupResponseCalculator(fhicl::ParameterSet const &);

#ifndef NO_ART
  std::unique_ptr<systtools::EventResponse> GetEventResponse(art::Event const &);
#endif

  std::string AsString();

private:
  bool applyToAll;
  std::unique_ptr<std::mt19937_64> RNgine;
  std::unique_ptr<std::normal_distribution<double>> RNJesus;
};

#endif
