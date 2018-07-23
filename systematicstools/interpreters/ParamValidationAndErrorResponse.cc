#include "ParamValidationAndErrorResponse.hh"

#include <cmath>
#include <iostream>

using namespace systtools;

double ParamValidationAndErrorResponse::CheckResponse(
    double r, SystParamHeader const &hdr, size_t idx) const {
  // Can only really check weight response as lateral systematics could be
  // anything.
  if (!hdr.isWeightSystematicVariation) {
    return r;
  }
  if (!fAllowNegativeWeights && (r < 0)) {
    if (fPedantry <= kMeh) {
      if (idx == std::numeric_limits<size_t>::max()) { // single value check
        std::cout << "[" << ((fPedantry == kNotOnMyWatch) ? "ERROR" : "WARN")
                  << "]: The weight response of parameter " << hdr.prettyName
                  << " can be negative: " << r
                  << ", and negative weights are not enabled." << std::endl;
      } else {
        std::cout << "[" << ((fPedantry == kNotOnMyWatch) ? "ERROR" : "WARN")
                  << "]: The weight response of parameter " << hdr.prettyName
                  << " for "
                  << (hdr.isSplineable ? "spline knot " : "variation ") << idx
                  << " is negative: " << r
                  << ", and negative weights are not enabled." << std::endl;
      }
      if (fPedantry == kNotOnMyWatch) {
        throw;
      }
    }
    // Force default
    switch (fErrorResponse) {
    case kZeroResponse: {
      r = 0;
      break;
    }
    case kUnityWeight: {
      r = 1;
      break;
    }
    case kBoundaryResponse: {
      r = 0;
      break;
    }
    }
  }

  if (fabs(r) < fSmallWeight) {
    if (fPedantry <= kMeh) {
      if (idx == std::numeric_limits<size_t>::max()) { // single value check
        std::cout << "[" << ((fPedantry == kNotOnMyWatch) ? "ERROR" : "WARN")
                  << "]: The weight response of parameter " << hdr.prettyName
                  << " is below the low weight limit set: |" << r << "| < "
                  << fSmallWeight << std::endl;
      } else {
        std::cout << "[" << ((fPedantry == kNotOnMyWatch) ? "ERROR" : "WARN")
                  << "]: The weight response of parameter " << hdr.prettyName
                  << " for "
                  << (hdr.isSplineable ? "spline knot " : "variation ") << idx
                  << " is below the low weight limit set: |" << r << "| < "
                  << fSmallWeight << std::endl;
      }
      if (fPedantry == kNotOnMyWatch) {
        throw;
      }
    }
    // Force default
    switch (fErrorResponse) {
    case kZeroResponse: {
      r = 0;
      break;
    }
    case kUnityWeight: {
      r = 1;
      break;
    }
    case kBoundaryResponse: {
      r = fSmallWeight;
      break;
    }
    }
  }

  if (fabs(r) > fLargeWeight) {
    if (fPedantry <= kMeh) {
      if (idx == std::numeric_limits<size_t>::max()) { // single value check
        std::cout << "[" << ((fPedantry == kNotOnMyWatch) ? "ERROR" : "WARN")
                  << "]: The weight response of parameter " << hdr.prettyName
                  << " is above the high weight limit set: |" << r << "| > "
                  << fLargeWeight << std::endl;
      } else {
        std::cout << "[" << ((fPedantry == kNotOnMyWatch) ? "ERROR" : "WARN")
                  << "]: The weight response of parameter " << hdr.prettyName
                  << " for "
                  << (hdr.isSplineable ? "spline knot " : "variation ") << idx
                  << " is above the high weight limit set: |" << r << "| > "
                  << fLargeWeight << std::endl;
      }
      if (fPedantry == kNotOnMyWatch) {
        throw;
      }
    }

    // Force default
    switch (fErrorResponse) {
    case kZeroResponse: {
      r = 0;
      break;
    }
    case kUnityWeight: {
      r = 1;
      break;
    }
    case kBoundaryResponse: {
      r = fLargeWeight;
      break;
    }
    }
  }

  return r;
}
