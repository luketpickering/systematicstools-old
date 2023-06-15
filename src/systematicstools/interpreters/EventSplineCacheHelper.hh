#ifndef SYSTTOOLS_INTERPRETERS_EVENTSPLINECACHEHELPER_SEEN
#define SYSTTOOLS_INTERPRETERS_EVENTSPLINECACHEHELPER_SEEN

#include "systematicstools/interpreters/ParamHeaderHelper.hh"
#include "systematicstools/interpreters/ParamValidationAndErrorResponse.hh"

#include <iostream>

namespace systtools {
typedef size_t eventId_t;

template <typename event_unit_t> class EventSplineCacheBase {

protected:
  param_value_map_t currentValues;
  param_list_t weightParams;
  param_list_t lateralParams;
  std::vector<std::pair<event_unit_t,
                        std::pair<ParamHeaderHelper::param_tspline_map_t,
                                  ParamHeaderHelper::param_tspline_map_t>>>
      fEvents;
  ParamHeaderHelper fHeaderHelper;
  ParamValidationAndErrorResponse fChkErr;

public:
  typedef std::vector<event_unit_t> event_t;

  EventSplineCacheBase(){};
  EventSplineCacheBase(param_header_map_t headers)
      : fHeaderHelper(headers), fChkErr{} {}
  EventSplineCacheBase(param_header_map_t &&headers)
      : fHeaderHelper(std::move(headers)), fChkErr{} {}

  void SetHeaders(param_header_map_t const &headers) {
    fHeaderHelper = ParamHeaderHelper(headers);
  }
  void SetHeaders(param_header_map_t &&headers) {
    fHeaderHelper = ParamHeaderHelper(std::move(headers));
  }

  void SetChkErr(ParamValidationAndErrorResponse const &ChkErr) {
    fHeaderHelper.SetChkErr(ChkErr);
    fChkErr = ChkErr;
  }

  ///\brief Take a copy of the event and build the internal splines from the
  /// supplied event information.
  eventId_t CacheEvent(event_unit_t const &eu,
                       event_unit_response_t const &eur) {
    param_list_t parameters;
    for (auto &pr : eur) {
      parameters.push_back(pr.first);
    }

    eventId_t id = fEvents.size();

    std::cout << "[INFO]: Caching event " << id << std::endl;

    for (auto &resp : eur) {
      std::cout << "\tParam " << resp.first << " has " << resp.second.size()
                << " responses. Is it known about by Event cache? "
                << currentValues.count(resp.first) << ", by the header helper? "
                << fHeaderHelper.HaveHeader(resp.first) << std::endl;
    }

    fEvents.emplace_back(
        eu, std::pair<ParamHeaderHelper::param_tspline_map_t,
                      ParamHeaderHelper::param_tspline_map_t>{{}, {}});
    std::cout << "[INFO]: Getting splines for " << parameters.size()
              << " parameters." << std::endl;
    for (auto &&isp : fHeaderHelper.GetSplines(parameters, eur)) {
      std::cout << "[INFO]: Adding spline for param " << isp.first << std::endl;
      if (fHeaderHelper.IsWeightResponse(isp.first)) {
        fEvents.back().second.first.emplace(isp.first, isp.second);
      } else {
        fEvents.back().second.second.emplace(isp.first, isp.second);
      }
    }
    return id;
  }
  // ///\brief Take the supplied event and build the internal splines from the
  // /// supplied event information.
  // ///
  // /// The response information in its current form is discarded after spline
  // /// creation, furthermore the TSpline3 implementation is not set up to take
  // /// advantage of move semantics, so ParamheaderHelper was not written to
  // allow
  // /// efficient spline creation through rvalue references.
  // eventId_t CacheEvent(event_unit_t &&eu, event_unit_response_t const &eur) {
  //   param_list_t parameters;
  //   for (auto &pr : eur) {
  //     parameters.push_back(pr.first);
  //   }
  //
  //   eventId_t id = fEvents.size();
  //
  //   std::cout << "[INFO]: Caching event " << id << std::endl;
  //
  //   for (auto &resp : eur) {
  //     std::cout << "\tParam " << resp.first << " has " << resp.second.size()
  //               << " responses. Is it known about by Event cache? "
  //               << currentValues.count(resp.first) << ", by the header
  //               helper? "
  //               << fHeaderHelper.HaveHeader(resp.first) << std::endl;
  //   }
  //
  //   fEvents.emplace_back(
  //       std::move(eu),
  //       std::pair<ParamHeaderHelper::param_tspline_map_t,
  //                 ParamHeaderHelper::param_tspline_map_t>{{}, {}});
  //   std::cout << "[INFO]: Getting splines for " << parameters.size()
  //             << " parameters." << std::endl;
  //   for (auto &&isp : fHeaderHelper.GetSplines(parameters, eur)) {
  //     std::cout << "[INFO]: Adding spline for param " << isp.first <<
  //     std::endl; if (fHeaderHelper.IsWeightResponse(isp.first)) {
  //       fEvents.back().second.first.emplace(isp.first, isp.second);
  //     } else {
  //       fEvents.back().second.second.emplace(isp.first, isp.second);
  //     }
  //   }
  //   return id;
  // }

  std::vector<eventId_t> CacheEvents(event_t const &e,
                                     EventResponse const &er) {
    std::vector<eventId_t> rtn;
    size_t NToAdd = e.size();
    if (e.size() != er.size()) {
      std::cout << "["
                << ((fChkErr.fPedantry ==
                     ParamValidationAndErrorResponse::kAnythingGoes)
                        ? "ERROR"
                        : "WARN")
                << "]: Attempting to cache events, but the number of events ("
                << e.size()
                << ") differs from the number of event responses passed ("
                << er.size() << ")." << std::endl;
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        throw;
      }
      NToAdd = std::min(e.size(), er.size());
    }
    for (size_t i = 0; i < NToAdd; ++i) {
      rtn.push_back(CacheEvent(e[i], er[i]));
    }
    return rtn;
  }
  std::vector<eventId_t> CacheEvents(event_t &&e, EventResponse &&er) {
    std::vector<eventId_t> rtn;
    size_t NToAdd = e.size();
    if (e.size() != er.size()) {
      std::cout << "["
                << ((fChkErr.fPedantry ==
                     ParamValidationAndErrorResponse::kAnythingGoes)
                        ? "ERROR"
                        : "WARN")
                << "]: Attempting to cache events, but the number of events ("
                << e.size()
                << ") differs from the number of event responses passed ("
                << er.size() << ")." << std::endl;
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        throw;
      }
      NToAdd = std::min(e.size(), er.size());
    }
    for (size_t i = 0; i < NToAdd; ++i) {
      rtn.push_back(CacheEvent(std::move(e[i]), er[i]));
    }
    return rtn;
  }

  size_t GetNEventsInCache() { return fEvents.size(); }

  void DeclareUsingParameter(paramId_t i, double v = kDefaultDouble) {
    if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
      if (!fHeaderHelper.HaveHeader(i)) {
        if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
          std::cout << "["
                    << ((fChkErr.fPedantry ==
                         ParamValidationAndErrorResponse::kNotOnMyWatch)
                            ? "ERROR"
                            : "WARN")
                    << "]: Attempted to declare the use of parameter " << i
                    << " but the header information has not been loaded."
                    << std::endl;
          if (fChkErr.fPedantry ==
              ParamValidationAndErrorResponse::kNotOnMyWatch) {
            throw;
          }
        }
        // Can carry on here as it may be loaded later.
      }
      // Check if the use of this parameter has already been declared.
      if (currentValues.find(i) != currentValues.end()) {
        if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
          std::cout << "["
                    << ((fChkErr.fPedantry ==
                         ParamValidationAndErrorResponse::kNotOnMyWatch)
                            ? "ERROR"
                            : "WARN")
                    << "]: Attempted to declare the use of parameter " << i
                    << " "
                    << (fHeaderHelper.HaveHeader(i)
                            ? fHeaderHelper.GetHeader(i).prettyName + " "
                            : "")
                    << "but it has already been declared." << std::endl;
          if (fChkErr.fPedantry ==
              ParamValidationAndErrorResponse::kNotOnMyWatch) {
            throw;
          }
        }
      }
    }
    currentValues[i] = v;
    if (fHeaderHelper.IsWeightResponse(i)) {
      weightParams.push_back(i);
    } else {
      lateralParams.push_back(i);
    }
  }
  void DeclareUsingParameters(param_value_map_t const &ivmap) {
    for (auto &iv : ivmap) {
      DeclareUsingParameter(iv.first, iv.second);
    }
  }
  void DeclareUsingParameters(param_list_t const &ilist) {
    for (auto &i : ilist) {
      DeclareUsingParameter(i);
    }
  }
  void SetParameterValue(paramId_t i, double v) {
    if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
      if (!fHeaderHelper.HaveHeader(i)) {
        if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
          std::cout << "["
                    << ((fChkErr.fPedantry ==
                         ParamValidationAndErrorResponse::kNotOnMyWatch)
                            ? "ERROR"
                            : "WARN")
                    << "]: Attempted to declare the use of parameter " << i
                    << " but the header information has not been loaded."
                    << std::endl;
          if (fChkErr.fPedantry ==
              ParamValidationAndErrorResponse::kNotOnMyWatch) {
            throw;
          }
        }
        // Can carry on here as it may be loaded later.
      }
      // Check if the use of this parameter has already been declared.
      if (currentValues.find(i) == currentValues.end()) {
        if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
          std::cout << "["
                    << ((fChkErr.fPedantry ==
                         ParamValidationAndErrorResponse::kNotOnMyWatch)
                            ? "ERROR"
                            : "WARN")
                    << "]: Attempted to set the value of parameter " << i << " "
                    << (fHeaderHelper.HaveHeader(i)
                            ? fHeaderHelper.GetHeader(i).prettyName + " "
                            : "")
                    << "but it has not been declared." << std::endl;
          if (fChkErr.fPedantry ==
              ParamValidationAndErrorResponse::kNotOnMyWatch) {
            throw;
          }
        }
      }
    }
    currentValues[i] = v;
    if (fHeaderHelper.IsWeightResponse(i)) {
      weightParams.push_back(i);
    } else {
      lateralParams.push_back(i);
    }
  }

  bool KnowAboutParameter(paramId_t i) {
    return (currentValues.find(i) != currentValues.end());
  }

  void SetParametersValue(param_value_map_t const &ivmap) {
    for (auto &iv : ivmap) {
      SetParameterValue(iv.first, iv.second);
    }
  }

  bool ParameterAffectsEventWeight(paramId_t i, eventId_t eid) {
    return (fEvents[eid].second.first.find(i) !=
            fEvents[eid].second.first.end());
  }
  bool ParameterAffectsEventLateral(paramId_t i, eventId_t eid) {
    return (fEvents[eid].second.second.find(i) !=
            fEvents[eid].second.second.end());
  }

  event_unit_t const &GetEventUnit(eventId_t eid) { return fEvents[eid].first; }
};

template <typename event_unit_t,
          ParamValidationAndErrorResponse::CareLevel CLtight =
              ParamValidationAndErrorResponse::kFrog,
          typename Enable = void>
class EventSplineCache : public EventSplineCacheBase<event_unit_t> {};

template <typename event_unit_t,
          ParamValidationAndErrorResponse::CareLevel CLtight>
class EventSplineCache<
    event_unit_t, CLtight,
    typename std::enable_if<CLtight == ParamValidationAndErrorResponse::kHare,
                            void>::type>
    : public EventSplineCacheBase<event_unit_t> {

  using EventSplineCacheBase<event_unit_t>::currentValues;
  using EventSplineCacheBase<event_unit_t>::weightParams;
  using EventSplineCacheBase<event_unit_t>::lateralParams;
  using EventSplineCacheBase<event_unit_t>::fEvents;
  using EventSplineCacheBase<event_unit_t>::fHeaderHelper;
  using EventSplineCacheBase<event_unit_t>::fChkErr;

  using EventSplineCacheBase<event_unit_t>::KnowAboutParameter;
  using EventSplineCacheBase<event_unit_t>::ParameterAffectsEventWeight;
  using EventSplineCacheBase<event_unit_t>::ParameterAffectsEventLateral;

public:
  double GetEventWeightResponse(paramId_t i, eventId_t eid, double v) {
    return ParameterAffectsEventWeight(i, eid)
               ? fEvents[eid].second.first[i].Eval(v)
               : 1;
  }

  double GetEventWeightResponse(paramId_t i, eventId_t eid) {
    return GetEventWeightResponse(i, eid, currentValues[i]);
  }

  double GetTotalEventWeightResponse(eventId_t eid) {
    double weight = 1;
    for (auto &i : weightParams) {
      GetEventWeightResponse(i, eid, currentValues[i]);
    }
    return weight;
  }
  double GetEventLateralResponse(paramId_t i, eventId_t eid, double v) {
    return ParameterAffectsEventLateral(i, eid)
               ? fEvents[eid].second.second[i].Eval(v)
               : 1;
  }
  double GetEventLateralResponse(paramId_t i, eventId_t eid) {
    return GetEventLateralResponse(i, eid, currentValues[i]);
  }
};

template <typename event_unit_t,
          ParamValidationAndErrorResponse::CareLevel CLtight>
class EventSplineCache<
    event_unit_t, CLtight,
    typename std::enable_if<CLtight == ParamValidationAndErrorResponse::kFrog,
                            void>::type>
    : public EventSplineCacheBase<event_unit_t> {

  using EventSplineCacheBase<event_unit_t>::currentValues;
  using EventSplineCacheBase<event_unit_t>::weightParams;
  using EventSplineCacheBase<event_unit_t>::lateralParams;
  using EventSplineCacheBase<event_unit_t>::fEvents;
  using EventSplineCacheBase<event_unit_t>::fHeaderHelper;
  using EventSplineCacheBase<event_unit_t>::fChkErr;

  using EventSplineCacheBase<event_unit_t>::KnowAboutParameter;
  using EventSplineCacheBase<event_unit_t>::ParameterAffectsEventWeight;
  using EventSplineCacheBase<event_unit_t>::ParameterAffectsEventLateral;

public:
  double GetEventWeightResponse(paramId_t i, eventId_t eid, double v) {
    if (fEvents.size() < eid) {
      std::cout << "["
                << (fChkErr.fPedantry ==
                            ParamValidationAndErrorResponse::kNotOnMyWatch
                        ? "ERROR"
                        : "WARN")
                << "]: Requested event " << eid << ", but only have "
                << fEvents.size() << " in the cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
      return ((fChkErr.fErrorResponse ==
               ParamValidationAndErrorResponse::kUnityWeight)
                  ? 1
                  : 0);
    }
    if (!KnowAboutParameter(i)) {
      std::cout << "["
                << (fChkErr.fPedantry ==
                            ParamValidationAndErrorResponse::kNotOnMyWatch
                        ? "ERROR"
                        : "WARN")
                << "]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }

      if (!fHeaderHelper.HaveHeader(i)) {
        std::cout << "["
                  << (fChkErr.fPedantry ==
                              ParamValidationAndErrorResponse::kNotOnMyWatch
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested event weight response for parameter "
                  << fHeaderHelper.GetHeader(i).prettyName
                  << ", but it is not understood by the currently loaded "
                     "parameter headers.."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
        return ((fChkErr.fErrorResponse ==
                 ParamValidationAndErrorResponse::kUnityWeight)
                    ? 1
                    : 0);
      }
    }
    return ParameterAffectsEventWeight(i, eid)
               ? fEvents[eid].second.first[i].Eval(v)
               : 1;
  }

  double GetEventWeightResponse(paramId_t i, eventId_t eid) {
    if (!KnowAboutParameter(i)) {
      std::cout << "[ERROR]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      throw;
    }
    return GetEventWeightResponse(i, eid, currentValues[i]);
  }

  double GetTotalEventWeightResponse(eventId_t eid) {
    double weight = 1;
    for (auto &i : weightParams) {
      GetEventWeightResponse(i, eid, currentValues[i]);
    }
    return weight;
  }
  double GetEventLateralResponse(paramId_t i, eventId_t eid, double v) {
    if (fEvents.size() < eid) {
      std::cout << "[ERROR]: Requested event " << eid << ", but only have "
                << fEvents.size() << " in the cache." << std::endl;
      return ((fChkErr.fErrorResponse ==
               ParamValidationAndErrorResponse::kUnityWeight)
                  ? 1
                  : 0);
    }
    if (!KnowAboutParameter(i)) {
      std::cout << "["
                << (fChkErr.fPedantry ==
                            ParamValidationAndErrorResponse::kNotOnMyWatch
                        ? "ERROR"
                        : "WARN")
                << "]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }

      if (!fHeaderHelper.HaveHeader(i)) {
        std::cout << "["
                  << (fChkErr.fPedantry ==
                              ParamValidationAndErrorResponse::kNotOnMyWatch
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested event weight response for parameter "
                  << fHeaderHelper.GetHeader(i).prettyName
                  << ", but it is not understood by the currently loaded "
                     "parameter headers.."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
        return ((fChkErr.fErrorResponse ==
                 ParamValidationAndErrorResponse::kUnityWeight)
                    ? 1
                    : 0);
      }
    }
    return ParameterAffectsEventLateral(i, eid)
               ? fEvents[eid].second.second[i].Eval(v)
               : 1;
  }
  double GetEventLateralResponse(paramId_t i, eventId_t eid) {
    if (!KnowAboutParameter(i)) {
      std::cout << "[ERROR]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      throw;
    }
    return GetEventLateralResponse(i, eid, currentValues[i]);
  }
};

template <typename event_unit_t,
          ParamValidationAndErrorResponse::CareLevel CLtight>
class EventSplineCache<
    event_unit_t, CLtight,
    typename std::enable_if<
        CLtight == ParamValidationAndErrorResponse::kTortoise, void>::type>
    : public EventSplineCacheBase<event_unit_t> {

  using EventSplineCacheBase<event_unit_t>::currentValues;
  using EventSplineCacheBase<event_unit_t>::weightParams;
  using EventSplineCacheBase<event_unit_t>::lateralParams;
  using EventSplineCacheBase<event_unit_t>::fEvents;
  using EventSplineCacheBase<event_unit_t>::fHeaderHelper;
  using EventSplineCacheBase<event_unit_t>::fChkErr;

  using EventSplineCacheBase<event_unit_t>::KnowAboutParameter;
  using EventSplineCacheBase<event_unit_t>::ParameterAffectsEventWeight;
  using EventSplineCacheBase<event_unit_t>::ParameterAffectsEventLateral;

public:
  double GetEventWeightResponse(paramId_t i, eventId_t eid, double v) {
    if (fEvents.size() < eid) {
      std::cout << "["
                << (fChkErr.fPedantry ==
                            ParamValidationAndErrorResponse::kNotOnMyWatch
                        ? "ERROR"
                        : "WARN")
                << "]: Requested event " << eid << ", but only have "
                << fEvents.size() << " in the cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
      return ((fChkErr.fErrorResponse ==
               ParamValidationAndErrorResponse::kUnityWeight)
                  ? 1
                  : 0);
    }
    if (!KnowAboutParameter(i)) {
      std::cout << "["
                << (fChkErr.fPedantry ==
                            ParamValidationAndErrorResponse::kNotOnMyWatch
                        ? "ERROR"
                        : "WARN")
                << "]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
      if (!fHeaderHelper.HaveHeader(i)) {
        std::cout << "["
                  << (fChkErr.fPedantry ==
                              ParamValidationAndErrorResponse::kNotOnMyWatch
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested event weight response for parameter "
                  << fHeaderHelper.GetHeader(i).prettyName
                  << ", but it is not understood by the currently loaded "
                     "parameter headers.."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
        return ((fChkErr.fErrorResponse ==
                 ParamValidationAndErrorResponse::kUnityWeight)
                    ? 1
                    : 0);
      }
    }

    if (fHeaderHelper.HasParameterLowLimit(i) &&
        (v < fHeaderHelper.GetParameterLowLimit(i))) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Parameter \"" << fHeaderHelper.GetHeader(i).prettyName
                  << "\", evaluated at " << v
                  << ", but specified lower bound at "
                  << fHeaderHelper.GetParameterLowLimit(i) << "." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }

      v = fHeaderHelper.GetParameterLowLimit(i);
    }
    if (fHeaderHelper.HasParameterUpLimit(i) &&
        (v > fHeaderHelper.GetParameterUpLimit(i))) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Parameter \"" << fHeaderHelper.GetHeader(i).prettyName
                  << "\", evaluated at " << v
                  << ", but specified upper bound at "
                  << fHeaderHelper.GetParameterUpLimit(i) << "." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }

      v = fHeaderHelper.GetParameterUpLimit(i);
    }

    return ParameterAffectsEventWeight(i, eid)
               ? fEvents[eid].second.first[i].Eval(v)
               : 1;
  }

  double GetEventWeightResponse(paramId_t i, eventId_t eid) {
    if (!KnowAboutParameter(i)) {
      std::cout << "[ERROR]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
      return ((fChkErr.fErrorResponse ==
               ParamValidationAndErrorResponse::kUnityWeight)
                  ? 1
                  : 0);
    }
    return GetEventWeightResponse(i, eid, currentValues[i]);
  }

  double GetTotalEventWeightResponse(eventId_t eid) {
    double weight = 1;
    for (auto &i : weightParams) {
      GetEventWeightResponse(i, eid, currentValues[i]);
    }
    return weight;
  }
  double GetEventLateralResponse(paramId_t i, eventId_t eid, double v) {
    if (fEvents.size() < eid) {
      std::cout << "["
                << (fChkErr.fPedantry ==
                            ParamValidationAndErrorResponse::kNotOnMyWatch
                        ? "ERROR"
                        : "WARN")
                << "]: Requested event " << eid << ", but only have "
                << fEvents.size() << " in the cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
      return ((fChkErr.fErrorResponse ==
               ParamValidationAndErrorResponse::kUnityWeight)
                  ? 1
                  : 0);
    }
    if (!KnowAboutParameter(i)) {
      std::cout << "["
                << (fChkErr.fPedantry ==
                            ParamValidationAndErrorResponse::kNotOnMyWatch
                        ? "ERROR"
                        : "WARN")
                << "]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
      if (!fHeaderHelper.HaveHeader(i)) {
        std::cout << "["
                  << (fChkErr.fPedantry ==
                              ParamValidationAndErrorResponse::kNotOnMyWatch
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested event weight response for parameter "
                  << fHeaderHelper.GetHeader(i).prettyName
                  << ", but it is not understood by the currently loaded "
                     "parameter headers.."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
        return ((fChkErr.fErrorResponse ==
                 ParamValidationAndErrorResponse::kUnityWeight)
                    ? 1
                    : 0);
      }
    }

    if (fHeaderHelper.HasParameterLowLimit(i) &&
        (v < fHeaderHelper.GetParameterLowLimit(i))) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Parameter \"" << fHeaderHelper.GetHeader(i).prettyName
                  << "\", evaluated at " << v
                  << ", but specified lower bound at "
                  << fHeaderHelper.GetParameterLowLimit(i) << "." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }

      v = fHeaderHelper.GetParameterLowLimit(i);
    }
    if (fHeaderHelper.HasParameterUpLimit(i) &&
        (v > fHeaderHelper.GetParameterUpLimit(i))) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Parameter \"" << fHeaderHelper.GetHeader(i).prettyName
                  << "\", evaluated at " << v
                  << ", but specified upper bound at "
                  << fHeaderHelper.GetParameterUpLimit(i) << "." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }

      v = fHeaderHelper.GetParameterUpLimit(i);
    }

    return ParameterAffectsEventLateral(i, eid)
               ? fEvents[eid].second.second[i].Eval(v)
               : 1;
  }
  double GetEventLateralResponse(paramId_t i, eventId_t eid) {
    if (!KnowAboutParameter(i)) {
      std::cout << "[ERROR]: Requested event weight response for parameter "
                << fHeaderHelper.GetHeader(i).prettyName
                << ", but it has not been declared to the Cache." << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
      return ((fChkErr.fErrorResponse ==
               ParamValidationAndErrorResponse::kUnityWeight)
                  ? 1
                  : 0);
    }
    return GetEventLateralResponse(i, eid, currentValues[i]);
  }
};

} // namespace systtools

#endif
