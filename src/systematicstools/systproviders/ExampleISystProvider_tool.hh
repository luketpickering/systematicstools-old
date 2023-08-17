#pragma once

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

  std::string AsString();

private:
  bool applyToAll;
  std::unique_ptr<std::mt19937_64> RNgine;
  std::unique_ptr<std::normal_distribution<double>> RNJesus;
};
