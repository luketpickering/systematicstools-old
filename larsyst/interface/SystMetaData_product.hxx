#ifndef METASYST_PRODUCT_HXX_SEEN
#define METASYST_PRODUCT_HXX_SEEN

#include <array>
#include <string>
#include <vector>

namespace larsyst {

struct SystParamHeader {
  ///\brief Human readable systematic parameter name
  std::string prettyName;
  ///\brief Unique identifier for this systematic parameter
  ///
  /// Used to key `std::map`-based event data product.
  ///\note Not guaranteed to persist between different configurations. i.e.
  ///`systParamId == 0` might be used for some physics model parameter in one
  /// data product and a calibration parameter in another.
  char systParamId;
  ///\brief Whether this systematic corresponds to a weight or property shift.
  ///
  ///\note Non-weight systematics will always need custom code on the part of
  /// a downstream consumer.
  bool isWeightSystematicVariation;
  ///\brief Whether the quantities stored in paramShiftValues and
  /// centralParamValue are in 'natural' units
  bool unitsAreNatural;
  ///\brief Whether the the response of this parameter is fully described by
  /// this meta-data
  ///
  /// Equivalent to `bool(Responses.size())`;
  bool differsEventByEvent;
  ///\brief The central parameter value used in this systematic evaluation.
  ///
  /// Respects unitsArNatural value.
  double centralParamValue;
  ///\brief The 'one sigma' shifts of this parameter, always defined in nautral
  /// units.
  ///
  /// Can be used by a downstream consumer to convert centralParamValue and
  /// paramShiftValues to and from natural units.
  std::array<double, 2> oneSigmaShifts;
  ///\brief Descriptor for interpreting `paramShiftValues` as a
  /// multi-dimensional spline
  ///
  /// For most parameters, the response to a variation is often independent from
  /// other parameter variations. In that case, this vector will be empty. For
  /// multi-parameter systematics this vector will contain the 'axes' of the
  /// flattened multi-dimensional array of parameter values stored in
  /// `paramShiftValues`, This works for multi-dimensional uniform grid-based
  /// splines, other forms of multi-dimensional systematic are not currently
  /// supported.
  std::vector<std::vector<double>> multiDimDescriptor;
  ///\brief Whether the paramShiftValues were chosen to facilitate a downstream
  /// consumer to spline the parameter response.
  ///
  /// When `isSplineable == false`, this parameter has likely been run in
  /// 'multisim' mode.
  bool isSplineable;
  ///\brief The shifted values that were calculated for this parameter.
  ///
  /// Contains the parameter values (either in sigma-shift units or natural
  /// units, see `oneSigmaShifts`) that were used to determine responses. The
  /// responses can either be event-level or parameter-level, parameter-level
  /// responses are stored in `responses`.
  std::vector<double> paramShiftValues;
  ///\brief The parameter responses for 'parameter-level' systematics.
  ///
  /// Empty for event-by-event parameters, contains universe or spline knot
  /// responses for dials that affect all events in the same way.
  ///
  /// These will most often be used for overall event-class re-normalisations,
  /// which do not need to be stored event-by-event.
  std::vector<double> responses;
};

/// The full metaData ResultsProduct including all systematic parameter headers.
struct SystMetaData {
  std::vector<SystParamHeader> headers;
};

} // namespace larsyst

#endif
