#pragma once

#include "systematicstools/interface/ISystProviderTool.hh"

#include <memory>
#include <random>
#include <string>

class ExampleISystProvider : public systtools::ISystProviderTool {
public:
  explicit ExampleISystProvider(fhiclsimple::ParameterSet const &);

  systtools::SystMetaData BuildSystMetaData(fhiclsimple::ParameterSet const &,
                                          systtools::paramId_t);
  fhiclsimple::ParameterSet GetExtraToolOptions();
  bool SetupResponseCalculator(fhiclsimple::ParameterSet const &);

  std::string AsString();

private:
  bool applyToAll;
  std::unique_ptr<std::mt19937_64> RNgine;
  std::unique_ptr<std::normal_distribution<double>> RNJesus;
};

