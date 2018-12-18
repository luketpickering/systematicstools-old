#ifndef SYSTTOOLS_INTERPRETERS_POLYRESPONSE_SEEN
#define SYSTTOOLS_INTERPRETERS_POLYRESPONSE_SEEN

#include "systematicstools/utility/ROOTUtility.hh"

#include <array>

namespace systtools {
template <size_t n> struct PolyResponse : std::array<double, n + 1> {

  PolyResponse(std::vector<double> const &xvals,
               std::vector<double> const &yvals)
      : std::array<double, n + 1>(GetPolyFitCoeffs<n>(xvals, yvals)) {}

  PolyResponse(std::array<double, n + 1> const &coeffs)
      : std::array<double, n + 1>(coeffs) {}

  PolyResponse(double const *coeffs) {
    for (size_t i = 0; i < (n + 1); ++i) {
      this->at(i) = coeffs[i];
    }
  }
  
  PolyResponse(PolyResponse &&other)
      : std::array<double, n + 1>(std::move(other)) {}

  double eval(double x) const {
    double val = 0;
    double xpow = 1;
    for (size_t dim = 0; dim < (n + 1); ++dim) {
      val += this->at(dim) * xpow;
      xpow *= x;
    }
    return val;
  }
};
} // namespace systtools

#endif
