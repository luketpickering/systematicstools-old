#ifndef SYSTTOOLS_INTERPRETERS_PARAMHEADERHELPER_SEEN
#define SYSTTOOLS_INTERPRETERS_PARAMHEADERHELPER_SEEN

#include "systematicstools/interface/SystMetaData.hh"
#include "systematicstools/interface/types.hh"

#include "systematicstools/interpreters/PolyResponse.hh"

#include "systematicstools/interpreters/ParamValidationAndErrorResponse.hh"

#include "TSpline.h"

namespace systtools {

class ParamHeaderHelper {

  param_header_map_t fHeaders;
  ParamValidationAndErrorResponse fChkErr;

public:
  typedef std::vector<double> spline_t;
  typedef std::map<paramId_t, TSpline3> param_tspline_map_t;
  typedef std::vector<double> discrete_variation_list_t;

  ///\brief Constructor for parameter header meta-data helper class.
  ///
  ///\note a param_header_map_t instance can be retrieved from a parameter headers FHiCL document by systtools::BuildParameterHeaders, found in utility/ParameterAndProviderConfigurationUtility.hh
  ///
  /// Headers can be set/overriden after construction by ParamHeaderHelper::SetHeaders.
  ParamHeaderHelper(param_header_map_t headers = {},
                    ParamValidationAndErrorResponse chkerrs =
                        ParamValidationAndErrorResponse())
      : fHeaders(headers), fChkErr(chkerrs) {}
  ParamHeaderHelper(param_header_map_t &&headers,
                    ParamValidationAndErrorResponse chkerrs =
                        ParamValidationAndErrorResponse())
      : fHeaders(std::move(headers)), fChkErr(chkerrs) {}

  void SetHeaders(param_header_map_t const &headers) { fHeaders = headers; }
  void SetHeaders(param_header_map_t &&headers) {
    fHeaders = std::move(headers);
  }
  param_header_map_t const &GetHeaders() { return fHeaders; }

  void SetChkErr(ParamValidationAndErrorResponse const &ChkErr) {
    fChkErr = ChkErr;
  }

  ///\brief Get the header object for parameter i
  SystParamHeader const &GetHeader(paramId_t i) const;
  ///\brief Whether parameter i is handled by this helper.
  bool HaveHeader(paramId_t) const;

  ///\brief Get the header object for parameter named, name
  SystParamHeader const &GetHeader(std::string const &name) const;
  ///\brief Whether parameter named, name, is handled by this helper.
  bool HaveHeader(std::string const &) const;
  ///\brief Get the paramId_t for for parameter named, name, if it doesn't
  /// exist, kParamUnhandled<paramId_t> is returned.
  paramId_t GetHeaderId(std::string const &name) const;

  ///\brief Get list of all handled parameter Ids.
  param_list_t GetParameters() const;

  ///\brief Whether the values of parameter i have been randomly thrown
  ///
  /// For non-spline style parameters for which this returns false, the
  /// variations are still discrete and accessed through the
  /// GetDiscreteResponse{s} interface, but it should be known by a consumer
  /// that they are not randomly distributed.
  bool IsThrownParam(paramId_t) const;

  ///\brief Whether responses to parameter i are retrevied directly, or induce
  /// response in another parameter.
  ///
  /// If false, use GetResponseParamId to determine which parameter variations
  /// of i are included in.
  bool IsResponselessParam(paramId_t) const;

  ///\brief Get parameter id that variations in parameter i induce a response
  /// on.
  ///
  ///\note At higher care levels, will check if parameter i is a responseless
  /// parameter.
  paramId_t GetResponseParamId(paramId_t) const;

  ///\brief Whether parameter i is a spline-style parameter
  ///
  /// For spline-style parameters,the GetSpline, GetParameterResponse, and
  /// GetTotalResponse methods should be used to get parameter response
  /// information for a given value or set of parameter-value pairs.
  bool IsSplineParam(paramId_t) const;

  ///\brief Whether values of parameter i are considered to be in units of sigma
  /// or in 'natural' units.
  bool ValuesAreInNaturalUnits(paramId_t) const;

  ///\brief Whether responses to variations in parameter i are characterised by
  /// an event weight or some lateral shift in observables.
  bool IsWeightResponse(paramId_t) const;

  ///\brief Whether parameter i has a bounded range of validity
  ///
  ///\note For higher pedantry levels, requesting this for a non spline
  /// parameter will constiute an error.
  bool HasParameterLimits(paramId_t) const;
  ///\brief Whether parameter i has a lower bound on the range of validity
  ///
  ///\note For higher pedantry levels, requesting this for a non spline
  /// parameter will constiute an error.
  bool HasParameterLowLimit(paramId_t) const;
  ///\brief Whether parameter i has an upper bound on the range of validity
  ///
  ///\note For higher pedantry levels, requesting this for a non spline
  /// parameter will constiute an error.
  bool HasParameterUpLimit(paramId_t) const;

  ///\brief Get the lower bound for on parameter i values.
  ///
  ///\note For higher pedantry levels, requesting this for a non-spline
  /// parameter will constiute an error.
  ///
  ///\note For higher pedantry levels, requesting this for a spline-type
  /// parameter that isn't bounded from below will constitute an error.
  double GetParameterLowLimit(paramId_t) const;
  ///\brief Get the upper bound for on parameter i values.
  ///
  ///\note For higher pedantry levels, requesting this for a non-spline
  /// parameter will constiute an error.
  ///
  ///\note For higher pedantry levels, requesting this for a spline-type
  /// parameter that isn't bounded from above will constitute an error.
  double GetParameterUpLimit(paramId_t) const;

  ///\brief Get a TSpline object for a given parameter for a given event from
  /// the passed vector of responses.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  TSpline3 GetSpline(paramId_t, spline_t const &event_responses = {}) const;
  ///\brief Get a TSpline object for a given parameter for a given event from
  /// the passed event unit response.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  TSpline3 GetSpline(paramId_t, event_unit_response_t const &) const;
  ///\brief Get a PolyResponse object for a given parameter, for a given event
  /// from the passed event unit response.
  ///
  ///\note Performs very few checks.
  template <size_t n>
  PolyResponse<n> GetPolyResponse(paramId_t i,
                                  event_unit_response_t const &eur) const {
    if (!IsSplineParam(i)) {
      throw;
    }
    return PolyResponse<n>(GetHeader(i).paramVariations,
                           GetParamElementFromContainer(eur, i).responses);
  }
  ///\brief Get all of the splines for parameter i from the passed event
  /// responses.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  std::vector<TSpline3> GetSplines(paramId_t, EventResponse const &) const;
  ///\brief Get a map of the parameter-spline responses for all parameters in
  /// passed list from the passed event unit response.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  param_tspline_map_t GetSplines(param_list_t const &,
                                 event_unit_response_t const &) const;
  ///\brief Get the splined parameter responses for each event unit in the
  /// passed event response.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  std::vector<param_tspline_map_t> GetSplines(param_list_t const &,
                                              EventResponse const &) const;

  ///\brief Gets the splined response for parameter i, set to value v, given the
  /// passed spline information.
  ///
  /// For events where the response is fully characterised in the header, the
  /// event_responses vector can be empty.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  ///
  ///\note This method is very inefficient for anything but the simplest tests,
  /// if you want to vary and fit event-splines, please use GetSpline{,s} and
  /// cache the splines for each event.
  ///
  /// Uses GetSpline internally
  double GetParameterResponse(paramId_t, double,
                              spline_t const &event_responses = {}) const;
  ///\brief Gets the splined response for parameter i, set to value v, given the
  /// passed event unit information.
  ///
  ///\note This shouldn't be used on non-weight parameters. For higher care
  /// levels this will be enforced.
  ///
  ///\note At higher care levels, the passing non-spline parameters will be
  /// checked for.
  ///
  ///\note This method is very inefficient for anything but the simplest tests,
  /// if you want to vary and fit event-splines, please use GetSpline{,s} and
  /// cache the splines for each event.
  ///
  /// Uses GetSpline internally
  double GetParameterResponse(paramId_t i, double v,
                              event_unit_response_t const &) const;
  ///\brief Gets the multiplicatively combined, splined response for all passed
  /// parameter-value pairs given the passed event unit information.
  ///
  ///\note This shouldn't be used on non-weight parameters. For higher care
  /// levels this will be enforced.
  ///
  ///\note At higher care levels, the passing non-spline parameters will be
  /// checked for.
  ///
  ///\note This method is very inefficient for anything but the simplest tests,
  /// if you want to vary and fit event-splines, please use GetSpline{,s} and
  /// cache the splines for each event.
  ///
  /// Uses GetSpline internally
  double GetTotalResponse(param_value_list_t const &,
                          event_unit_response_t const &) const;

  ///\brief Gets the splined response for parameter i, set to value v, for each
  /// event unit in the passed event response.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  ///
  ///\note This method is very inefficient for anything but the simplest tests,
  /// if you want to vary and fit event-splines, please use GetSpline{,s} and
  /// cache the splines for each event.
  ///
  /// Uses GetSpline internally
  std::vector<double> GetParameterResponse(paramId_t, double,
                                           EventResponse const &) const;
  ///\brief Gets the multiplicatively combined, splined response for all passed
  /// parameter-value pairs separately for each event unit in the passed event
  /// response.
  ///
  ///\note This shouldn't be used on non-weight parameters. For higher care
  /// levels this will be enforced.
  ///
  ///\note At higher care levels, the passing of non-spline parameters will
  /// checked for.
  ///
  ///\note This method is very inefficient for anything but the simplest tests,
  /// if you want to vary and fit event-splines, please use GetSpline{,s} and
  /// cache the splines for each event.
  ///
  /// Uses GetSpline internally
  std::vector<double> GetTotalResponse(param_value_list_t const &,
                                       EventResponse const &) const;

  ///\brief Gets the number of variations for parameter i
  size_t GetNDiscreteVariations(paramId_t) const;
  ///\brief Gets the number of variations for each parameter
  /// in the passed parameter list.
  std::vector<size_t> GetNDiscreteVariations(param_list_t const &) const;

  ///\brief Gets the list of responses of parameter i from the passed mutlisim
  /// response information.
  ///
  ///\note For parameters that produce a different response for each event, this
  /// method essentially returns the input event_responses. It will apply
  /// validity checks based on the current values of fCare and fPedantry and
  /// possibly truncate bad responses based on fErrorResponse.
  ///
  ///\note This method can be of practical use for parameters which do not
  /// effect a response that differs event by event.
  discrete_variation_list_t
  GetDiscreteResponses(paramId_t, discrete_variation_list_t const &) const;

  ///\brief Gets the list of responses of parameter i from the passed mutlisim
  /// response information.
  ///
  ///\note For parameters that produce a different response for each event, this
  /// method essentially returns the input event_responses. It will apply
  /// validity checks based on the current values of fCare and fPedantry and
  /// possibly truncate bad responses based on fErrorResponse.
  ///
  ///\note This method can be of practical use for parameters which do not
  /// effect a response that differs event by event.
  ///
  ///\note At higher care levels the existance of parameter in event unit
  /// response will be checked for.
  discrete_variation_list_t
  GetDiscreteResponses(paramId_t, event_unit_response_t const &eur = {}) const;

  ///\brief Gets the response at variation j of parameter i from the passed
  /// mutlisim
  /// response information.
  ///
  ///\note For parameters that produce a different response for each event,
  /// this
  /// method essentially returns event_responses[j]
  ///
  ///\note At higher care levels, the passing of spline parameters will
  /// checked
  /// for.
  double GetDiscreteResponse(
      paramId_t, size_t j,
      discrete_variation_list_t const &event_responses = {}) const;

  ///\brief Gets the response at variation j of parameter i from the passed
  /// event unit response information.
  ///
  ///\note For parameters that produce a different response for each event, this
  /// method essentially returns event_responses[j]
  double GetDiscreteResponse(paramId_t i, size_t j,
                             event_unit_response_t const &) const;
  ///\brief Gets the multiplicativly combined response at variation j of each
  /// parameter in the passed parameter list from the passed event unit response
  /// information.
  double GetDiscreteResponse(param_list_t const &, size_t j,
                             event_unit_response_t const &) const;

  ///\brief Gets the response at variation j of parameter i for each event unit
  /// from
  /// the event response information.
  std::vector<double> GetDiscreteResponses(paramId_t, size_t j,
                                           EventResponse const &) const;
  ///\brief Gets the multiplicatively combined responses at variation j of the
  /// passed parameter set for each event unit from the event response
  /// information.
  ///
  ///\note This shouldn't be used on non-weight parameters. For higher care
  /// levels this will be enforced.
  std::vector<double> GetDiscreteResponses(param_list_t const &, size_t j,
                                           EventResponse const &) const;

  ///\brief Gets the response to all variations, for all events in the passed
  /// event response information.
  std::vector<discrete_variation_list_t>
  GetAllDiscreteResponses(paramId_t, EventResponse const &) const;

  ///\brief Gets the multiplicatively combined responses to all variations, for
  /// all passed parameters, for all events in the passed event response
  /// information.
  ///
  ///\note This shouldn't be used on non-weight parameters. For higher care
  /// levels this will be enforced.
  std::vector<discrete_variation_list_t>
  GetAllDiscreteResponses(param_list_t const &, EventResponse const &) const;

  ///\brief Gets the thrown parameter values for all parameters specified in the
  /// passed parameter list.
  std::map<paramId_t, discrete_variation_list_t>
  GetDiscreteVariationParameterValues(param_list_t const &) const;

  ///\brief How carefully to check parameter usage.
  ///
  ///\note This defines what usage checks should be run, as opposed to fPedanty,
  /// which defines the reaction to failed checks.
  ///
  /// * kTortoise: Check that parameters exist, check spline and variation
  /// number ranges are within bounds, for weight systematics check whether
  /// weights are too large or too small or negative, check that when getting a
  /// multiplicatively combined response, all of the responses are weight
  /// systematics.
  /// * kFrog: Check that parameters exist and are used correctly (spline type).
  /// * kHare: Assume everything is correct.
  void SetCareLevel(ParamValidationAndErrorResponse::CareLevel c) {
    fChkErr.fCare = c;
  }
  ///\brief How to react to the result of usage checks
  ///
  ///\note This defines how to react to failed checks, as opposed to fCare,
  /// which defines which checks to run.
  ///
  /// * kAnythingGoes: For bad parameter usage or response, return the default
  /// response.
  /// * kMeh: For bad parameter usage or response, post a warning, and return
  /// the default response.
  /// * kNotOnMyWatch: For bad parameter usage or response, post an error and
  /// throw an exception detailing the bad usage or response.
  void SetPedantLevel(ParamValidationAndErrorResponse::PedantLevel p) {
    fChkErr.fPedantry = p;
  }
  ///\brief How responses to failed checks are handled for fPedantry !=
  /// kNotOnMyWatch
  ///
  /// * kZeroResponse: Return 0 response
  /// * kUnityWeight: Return 0 response for non-weight systematics and a
  /// response weight of 1 for weight systematics.
  /// * kBoundaryResponse: If the error is out of bounds, return the response of
  /// the closest boundary.
  void
  SetErrorResponseLevel(ParamValidationAndErrorResponse::ErrorResponseLevel e) {
    fChkErr.fErrorResponse = e;
  }
  ///\brief Whether negative weights are allowed, ignored for non-weight
  /// systematics.
  void SetAllowNegativeWeights(bool a) { fChkErr.fAllowNegativeWeights = a; }
  ///\brief Weights further from 0 than this will be considered 'too large' for
  /// error checking purposes.
  void SetLargeWeightBoundary(double l) { fChkErr.fLargeWeight = l; }
  ///\brief Weights closer to 0 than this will be considered 'too small' for
  /// error checking purposes.
  void SetSmallWeightBoundary(double s) { fChkErr.fSmallWeight = s; }

  std::string GetHeaderInfo() const;
  std::string GetEventResponseInfo(event_unit_response_t) const;

private:
  ///\brief Used internally to skip getting a header that we have already got.
  ///
  /// Probably reeks of premature optimization.
  TSpline3 GetSpline(paramId_t, spline_t const &event_responses,
                     SystParamHeader const &) const;

  ///\brief Used internally to skip getting a header that we have already got.
  ///
  /// Probably reeks of premature optimization.
  ///
  ///\note At higher care levels checks before assuming parameter is in event
  /// unit response.
  TSpline3 GetSpline(paramId_t, event_unit_response_t const &,
                     SystParamHeader const &) const;

  ///\brief Used internally to skip getting a header that we have already got.
  ///
  /// Probably reeks of premature optimization.
  discrete_variation_list_t
  GetDiscreteResponses(paramId_t, discrete_variation_list_t const &,
                       SystParamHeader const &) const;
  ///\brief Used internally to skip getting a header that we have already got.
  ///
  /// Probably reeks of premature optimization.
  ///
  ///\note At higher care levels checks before assuming parameter is in event
  /// unit response.
  discrete_variation_list_t
  GetDiscreteResponses(paramId_t i, event_unit_response_t const &eur,
                       SystParamHeader const &hdr) const;

  ///\brief Checks parameter-value map for parameter mis-use
  ///
  /// Checks for bad parameters in a parameter map and acts accordingly.
  param_value_list_t CheckParamValueList(param_value_list_t) const;

  ///\brief Checks parameter list of parameter mis-use
  ///
  /// Checks for bad parameters in a parameter list and acts accordingly.
  ///
  /// Optionally checks for splineable parameters in the list.
  ///
  /// Optionally checks for weight-only responses, useful when returning a total
  /// weight
  param_list_t CheckParamList(param_list_t, bool ExpectSpline,
                              bool RequireWeightResponse) const;

  ///\brief Empty header
  ///
  /// Used when a reference to a unhandled header is expected but fPedantry is
  /// set low.
  static SystParamHeader nullheader;

  mutable spline_t scratch_spline_t1;
  mutable spline_t scratch_spline_t2;
  mutable discrete_variation_list_t scratch_discrete_variation_list_t1;
}; // namespace systtools
} // namespace systtools
#endif
