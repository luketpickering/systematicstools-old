#ifndef LARSYST_INTERFACE_ISYSTPROVIDERTOOL_SEEN
#define LARSYST_INTERFACE_ISYSTPROVIDERTOOL_SEEN

#include "EventResponse_product.hh"
#include "SystMetaData.hh"

#include "larsyst/interpreters/load_parameter_headers.hh"
#include "larsyst/utility/md5.hh"
#include "larsyst/utility/printers.hh"

#include "art/Framework/Principal/Event.h"

#include "fhiclcpp/ParameterSet.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <string>

namespace larsyst {

class ISystProvider_tool {
public:
  ISystProvider_tool(fhicl::ParameterSet const &ps)
      : fToolType{ps.get<std::string>("tool_type")}, fSeedSuggestion{0},
        fIsFullyConfigured{false}, fHaveMetaData{false} {
    if (!ps.has_key("uniqueName")) {
      fInstanceName = "";
      fFQName = fToolType;
    } else {
      fInstanceName = ps.get<std::string>("uniqueName");
      fFQName = fToolType + "_" + fInstanceName;
    }
  }

  bool ParamIsHandled(paramId_t i) {
    return GetParameterHeaderMetaDataIndex(i) != kParamUnhandled<size_t>;
  }
  paramId_t GetParameterId(std::string const &prettyName) {
    CheckHaveMetaData();
    for (auto &sph : fMetaData.headers) {
      if (sph.prettyName == prettyName) {
        return sph.systParamId;
      }
    }
    return kParamUnhandled<paramId_t>;
  }
  bool ParamIsHandled(std::string const &prettyName) {
    return GetParameterId(prettyName) != kParamUnhandled<paramId_t>;
  }
  ///\brief Get the number of variations to be calculated for parameter i
  size_t GetNVariations(paramId_t i) {
    SystParamHeader const &sph = Header(i);
    return sph.paramVariations.size();
  }

  ///\brief Allows set up method to suggest RNG seeds.
  ///
  /// This stops many syst providers being set up in quick succession all using
  /// similar seeds.
  void SuggestSeed(uint64_t seed) { fSeedSuggestion = seed; }

  ///\brief Allows a meta provider to be written that delegates well-correlated
  /// throws to mutliple chiild providers.
  ///
  /// Unfortunately must be public as sub classes do not get access to protected
  /// member functions via a base class pointer (as they may actually call a
  /// protected member of another subclass).
  virtual void SuggestParameterThrows(parameter_throws_list_t &&throws,
                                      bool Check = false) {
    std::cout
        << "[ERROR]: Attempted to suggest parameter throws to provider tool "
        << std::quoted(GetToolType())
        << ", but it cannot handle suggested throws." << std::endl;
    throw;
  };

  ///\brief Convert arbitrary configuration fhicl parameter set into generic
  /// systematic meta data.
  ///
  virtual SystMetaData ConfigureFromFHICL(fhicl::ParameterSet const &,
                                          paramId_t) = 0;

  virtual SystMetaData
  GenerateSystSetConfiguration(fhicl::ParameterSet const &ps, paramId_t id) {
    fMetaData = this->ConfigureFromFHICL(ps, id);
    for (auto &hdr : fMetaData.headers) {
      if (hdr.systParamId != id) {
        std::cout << "[ERROR]: Provider "
                  << std::quoted(GetFullyQualifiedName())
                  << " failed to set parameter " << std::quoted(hdr.prettyName)
                  << " to id " << id << " != " << hdr.systParamId << std::endl;
        throw;
      }
      id++;
    }
    fHaveMetaData = true;
    return GetSystSetConfiguration();
  }

  SystMetaData GetSystSetConfiguration() {
    CheckHaveMetaData();
    return fMetaData;
  }

  /// Try and read parameter configuration from input fhicl file.
  ///
  /// After reading parameters, the pure virtual Configure method is called for
  /// any final subclass configuration.
  ///
  ///\note sub-classes may not alter fMetaData during the configure call. This
  /// is checked for by md5-ing the stringified fhicl representation of the
  /// parameters before and after the call.
  bool ReadParameterHeaders(fhicl::ParameterSet const &ps) {
    std::vector<std::string> const &paramHeadersToRead =
        ps.get<std::vector<std::string>>("parameterHeaders");

    for (auto const &ph : paramHeadersToRead) {
      fMetaData.headers.emplace_back(larsyst::build_header_from_parameter_set(
          ps.get<fhicl::ParameterSet>(ph)));
    }
    fHaveMetaData = true;

    std::vector<std::string> parametermd5s;
    SystMetaData mdCopy = fMetaData;
    for (auto const &sph : fMetaData.headers) {
      parametermd5s.push_back(md5(to_str(sph)));
    }
    // Meta data loaded, now run any additonal subclass configuration.
    fIsFullyConfigured = this->Configure();
    size_t hdrctr = 0;
    for (auto const &sph : fMetaData.headers) {
      std::string digest = md5(to_str(sph));
      if (parametermd5s[hdrctr] != digest) {
        std::cout << "[ERROR]: MD5 of parameter #" << hdrctr << "("
                  << std::quoted(parametermd5s[hdrctr]) << ") was changed by "
                  << GetToolType() << "::Configure to " << std::quoted(digest)
                  << "." << std::endl;
        std::cout << "BEFORE: " << to_str(sph, false) << std::endl;
        std::cout << "AFTER: " << to_str(mdCopy.headers[hdrctr], false)
                  << std::endl;
        throw;
      }
      hdrctr++;
    }

    std::cout << "[INFO]: Syst provider configured " << fMetaData.headers.size()
              << " parameters." << GetFullyQualifiedName() << std::endl;
    for (auto const &sph : fMetaData.headers) {
      std::cout << to_str(sph) << std::endl;
    }
    std::cout << "============================================================="
              << std::endl;

    return fIsFullyConfigured;
  }

  virtual std::unique_ptr<EventResponse> GetEventResponse(art::Event &) = 0;

  std::string const &GetToolType() const { return fToolType; }
  std::string const &GetFullyQualifiedName() const { return fFQName; }
  std::string const &GetInstanceName() const { return fInstanceName; }

  virtual std::string AsString() = 0;

  virtual ~ISystProvider_tool(){};

protected:
  ///\brief Any further configuration required by a subclass before
  /// GetEventResponse can be called.
  ///
  /// This is meant for setting up slave weight calculators that are required to
  /// calculate responses but not for parameter variation re-interpretation.
  ///
  ///\note No configurations to fMetaData should be made by subclasses in here,
  /// all header information required by downstream interpreters must be built
  /// by ConfigureFromFHICL. This is enforced by md5-ing each parameter header
  /// before and after the call to Configure.
  virtual bool Configure() = 0;

  void CheckHaveMetaData(paramId_t i = kParamUnhandled<paramId_t>) {
    if (!fHaveMetaData) {
      std::cout
          << "[ERROR]: Requested syst set configuration from syst provider "
          << GetFullyQualifiedName() << ", but it has not been generated yet."
          << std::endl;
      throw;
    }
    if (i != kParamUnhandled<paramId_t>) {
      if (!ParamIsHandled(i)) {
        std::cout << "[ERROR]: SuggestParameterThrows Check failed. Parameter "
                     "with id \""
                  << i << "\", is not handled by this systematic provider: \""
                  << GetFullyQualifiedName() << "\"." << std::endl;
        throw;
      }
    }
  }

  size_t GetParameterHeaderMetaDataIndex(paramId_t i) {
    CheckHaveMetaData();
    if (i == kParamUnhandled<paramId_t>) {
      return kParamUnhandled<size_t>;
    }

    size_t index = 0;
    for (auto &sph : fMetaData.headers) {
      if (sph.systParamId == i) {
        return index;
      }
    }
    return kParamUnhandled<size_t>;
  }

  SystParamHeader const &Header(paramId_t i) {
    if (i == kParamUnhandled<paramId_t>) {
      std::cout << "[ERROR](" << __FILE__ << ":" << __LINE__
                << "): Syst provider requested parameter header information "
                   "for the unhandled parameter magic id. This is an internal "
                   "logic error and should not have happened."
                << std::endl;
      throw;
    }
    CheckHaveMetaData(i);
    return fMetaData.headers[GetParameterHeaderMetaDataIndex(i)];
  }

  std::string fToolType;
  std::string fInstanceName;
  std::string fFQName;

  uint64_t fSeedSuggestion;

  SystMetaData fMetaData;
  bool fIsFullyConfigured;

private:
  bool fHaveMetaData;
};

} // namespace larsyst

#endif
