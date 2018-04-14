////////////////////////////////////////////////////////////////////////
// Class:       larmetasyst
// Plugin Type: resultsproducer (art v2_10_03)
// File:        larmetasyst_plugin.cc
//
// Generated at Fri Apr 13 12:16:38 2018 by Luke Pickering using cetskelgen
// from cetlib version v3_02_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/ResultsProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Results.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

class larmetasyst;


class larmetasyst : public art::ResultsProducer {
public:
  explicit larmetasyst(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  larmetasyst(larmetasyst const &) = delete;
  larmetasyst(larmetasyst &&) = delete;
  larmetasyst & operator = (larmetasyst const &) = delete;
  larmetasyst & operator = (larmetasyst &&) = delete;

  // Required functions.
  void clear() override;
  void writeResults(art::Results & res) override;

private:

  // Declare member data here.

};


larmetasyst::larmetasyst(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
}

void larmetasyst::clear()
{
  // Implementation of required member function here.
}

void larmetasyst::writeResults(art::Results & res)
{
  // Implementation of required member function here.
}

DEFINE_ART_RESULTS_PLUGIN(larmetasyst)
