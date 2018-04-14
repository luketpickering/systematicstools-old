////////////////////////////////////////////////////////////////////////
// Class:       lareventsyst
// Plugin Type: producer (art v2_10_03)
// File:        lareventsyst_module.cc
//
// Generated at Fri Apr 13 12:16:24 2018 by Luke Pickering using cetskelgen
// from cetlib version v3_02_00.
////////////////////////////////////////////////////////////////////////

#include "larsyst/interface/ISystProvider.hxx"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>

class lareventsyst;

class lareventsyst : public art::EDProducer {
public:
  explicit lareventsyst(fhicl::ParameterSet const &p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  lareventsyst(lareventsyst const &) = delete;
  lareventsyst(lareventsyst &&) = delete;
  lareventsyst &operator=(lareventsyst const &) = delete;
  lareventsyst &operator=(lareventsyst &&) = delete;

  // Required functions.
  void produce(art::Event &e) override;

private:
  // Declare member data here.
};

lareventsyst::lareventsyst(fhicl::ParameterSet const &p)
// :
// Initialize member data here.
{
  // Call appropriate produces<>() functions here.

  std::cout << "[INFO]: Initializing..." << std::endl;
  larsyst::ISystProvider *is =
      larsyst::SystProviderFactory::Create("NuInteractSyst_CoulombCorrection");
      (void)is;
}

void lareventsyst::produce(art::Event &e) {
  // Implementation of required member function here.
  std::cout << "[INFO]: Hello!" << std::endl;
}

DEFINE_ART_MODULE(lareventsyst)
