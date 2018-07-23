#ifndef SYSTTOOLS_INTERPRETERS_PARAMVALIDIATIONANDERRORRESPONSE_SEEN
#define SYSTTOOLS_INTERPRETERS_PARAMVALIDIATIONANDERRORRESPONSE_SEEN

#include "systematicstools/interface/SystMetaData.hh"

#include <limits>

struct ParamValidationAndErrorResponse {
  ParamValidationAndErrorResponse()
      : fCare(kFrog), fPedantry(kMeh), fErrorResponse(kUnityWeight),
        fAllowNegativeWeights(false),
        fLargeWeight(std::numeric_limits<double>::max()), fSmallWeight(0) {}
  enum CareLevel { kTortoise = -1, kFrog = 0, kHare = 1 };
  ///\brief How carefully to check parameter usage.
  ///
  ///\note This defines what usage checks should be run, as opposed to fPedanty,
  /// which defines the reaction to failed checks.
  ///
  /// * kTortoise: Check that parameters exist, check spline and number of throw
  /// ranges are within bounds, for weight systematics check whether weights are
  /// too large or too small or negative, check that when getting a
  /// multiplicatively combined response, all of the responses are weight
  /// systematics.
  /// * kFrog: Check that parameters exist and are used correctly (spline
  /// vs. thrown).
  /// * kHare: Assume everything is correct.
  CareLevel fCare;
  void SetCareLevel(CareLevel c) { fCare = c; }

  enum PedantLevel { kNotOnMyWatch = -1, kMeh = 0, kAnythingGoes = 1 };
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
  PedantLevel fPedantry;
  void SetPedantLevel(PedantLevel p) { fPedantry = p; }

  enum ErrorResponseLevel {
    kZeroResponse = 0,
    kUnityWeight,
    kBoundaryResponse
  };
  ///\brief How responses to failed checks are handled for fPedantry !=
  /// kNotOnMyWatch
  ///
  /// * kZeroResponse: Return 0 response
  /// * kUnityWeight: Return 0 response for non-weight systematics and a
  /// response weight of 1 for weight systematics.
  /// * kBoundaryResponse: If the error is out of bounds, return the response of
  /// the closest boundary.
  ErrorResponseLevel fErrorResponse;
  void SetErrorResponseLevel(ErrorResponseLevel e) { fErrorResponse = e; }

  ///\brief Whether negative weights are allowed, ignored for non-weight
  /// systematics.
  bool fAllowNegativeWeights;
  void SetAllowNegativeWeights(bool a) { fAllowNegativeWeights = a; }
  ///\brief Weights further from 0 than this will be considered 'too large' for
  /// error checking purposes.
  double fLargeWeight;
  void SetLargeWeightBoundary(double l) { fLargeWeight = l; }

  ///\brief Weights closer to 0 than this will be considered 'too small' for
  /// error checking purposes.
  double fSmallWeight;
  void SetSmallWeightBoundary(double s) { fSmallWeight = s; }

  ///\brief Checks a response for validity.
  ///
  /// Checks with the current settings of fSmallWeight, fLargeWeight,
  /// fAllowNegativeWeights, fCare and responds according to fPedanty.
  ///
  ///\note If and index is passed, then the response is determined to be
  /// from a spline or a multisim parameter, and any error messages tailored.
  double CheckResponse(double, systtools::SystParamHeader const &,
                       size_t idx = std::numeric_limits<size_t>::max()) const;
};

#endif
