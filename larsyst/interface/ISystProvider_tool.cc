#include "larsyst/interface/ISystProvider_tool.hh"

namespace larsyst {

ISystProvider_tool::ISystProvider_tool(fhicl::ParameterSet const &ps)
    : fToolType{ps.get<std::string>("tool_type")}, fSeedSuggestion{0},
      fIsFullyConfigured{false}, fHaveSystMetaData{false} {
  if (!ps.has_key("instance_name")) {
    fInstanceName = "";
    fFQName = fToolType;
  } else {
    fInstanceName = ps.get<std::string>("instance_name");
    fFQName = fToolType + "_" + fInstanceName;
  }
}

paramId_t ISystProvider_tool::GetParameterId(std::string const &prettyName) {
  return GetParamId(fSystMetaData, prettyName);
}

void ISystProvider_tool::SuggestSeed(uint64_t seed) {
  if (fHaveSystMetaData) {
    throw ISystProvider_tool_seed_suggestion_post_configure()
        << "[ERROR]: Seed was suggested to ISystProvider: "
        << std::quoted(GetFullyQualifiedName()) << " after configuration.";
  }
  fSeedSuggestion = seed;
}

void ISystProvider_tool::SuggestParameterThrows(
    parameter_throws_list_t &&throws, bool Check) {
  throw ISystProvider_tool_method_unimplemented()
      << "[ERROR]: Attempted to suggest parameter throws to provider tool "
      << std::quoted(GetToolType())
      << ", but it doesn't handle suggested throws.";
}

void ISystProvider_tool::ConfigureFromToolConfig(fhicl::ParameterSet const &ps,
                                                 paramId_t firstId) {
  fSystMetaData = this->BuildSystMetaData(ps, firstId);

  // The following check expects them to be ordered, but the provider isn't
  // under any obligation to order them.
  std::stable_sort(fSystMetaData.begin(), fSystMetaData.end(),
                   [](SystParamHeader const &l, SystParamHeader const &r) {
                     return l.systParamId < r.systParamId;
                   });

  for (auto &hdr : fSystMetaData) {
    if (hdr.systParamId != firstId) {
      throw ISystProvider_tool_noncontiguous_parameter_Ids()
          << "[ERROR]: Provider " << std::quoted(GetFullyQualifiedName())
          << " failed to set parameter " << std::quoted(hdr.prettyName)
          << " to firstId " << firstId << " != " << hdr.systParamId;
    }
    firstId++;
  }
  fHaveSystMetaData = true;
}

SystMetaData const &ISystProvider_tool::GetSystMetaData() {
  CheckHaveMetaData();
  return fSystMetaData;
}

fhicl::ParameterSet ISystProvider_tool::GetParameterHeadersDocument() {

  CheckHaveMetaData();

  fhicl::ParameterSet ParamHeadersDoc;
  std::vector<std::string> HeaderKeys;
  for (auto &hdr : GetSystMetaData()) {
    ParamHeadersDoc.put(hdr.prettyName, SystParamHeaderToFHiCL(hdr));
    HeaderKeys.push_back(hdr.prettyName);
  }
  ParamHeadersDoc.put("parameter_headers", HeaderKeys);
  ParamHeadersDoc.put("tool_type", GetToolType());
  if (GetInstanceName().size()) {
    ParamHeadersDoc.put("instance_name", GetInstanceName());
  }
  fhicl::ParameterSet ToolOptions = GetExtraToolOptions();
  if (!ToolOptions.is_empty()) {
    ParamHeadersDoc.put("tool_options", ToolOptions);
  }

  return ParamHeadersDoc;
}

bool ISystProvider_tool::ConfigureFromParameterHeaders(
    fhicl::ParameterSet const &ps) {
  std::vector<std::string> const &ParamHeaderNames =
      ps.get<std::vector<std::string>>("parameter_headers");

  for (auto const &paramName : ParamHeaderNames) {
    fSystMetaData.emplace_back(larsyst::FHiCLToSystParamHeader(
        ps.get<fhicl::ParameterSet>(paramName)));
  }
  fHaveSystMetaData = true;

  fhicl::ParameterSet ToolOptions;
  ps.get_if_present("tool_options", ToolOptions);

  fIsFullyConfigured = this->SetupResponseCalculator(ToolOptions);

  std::cout << "[INFO]: Syst provider " << std::quoted(GetFullyQualifiedName())
            << " configured " << fSystMetaData.size() << " parameters."
            << std::endl;

  return fIsFullyConfigured;
}

void ISystProvider_tool::CheckHaveMetaData(paramId_t i) {
  if (!fHaveSystMetaData) {
    throw ISystProvider_tool_metadata_not_generated()
        << "[ERROR]: Requested syst set configuration from syst provider "
        << GetFullyQualifiedName() << ", but it has not been generated yet.";
  }
  if (i != kParamUnhandled<paramId_t>) {
    if (!ParamIsHandled(i)) {
      throw ISystProvider_tool_metadata_not_generated()
          << "[ERROR]: SuggestParameterThrows Check failed. Parameter "
             "with id \""
          << i << "\", is not handled by this systematic provider: \""
          << GetFullyQualifiedName() << "\".";
    }
  }
}

} // namespace larsyst
