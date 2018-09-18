#include "ParamHeaderHelper.hh"

#include "systematicstools/utility/printers.hh"

#include <iostream>
#include <utility>

namespace systtools {

// #define DEBUG_PARAMHEADERHELPER

SystParamHeader ParamHeaderHelper::nullheader = SystParamHeader();

SystParamHeader const &ParamHeaderHelper::GetHeader(paramId_t i) const {
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!HaveHeader(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << i
                  << ", but it is not currently configured." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return nullheader;
    }
  }
  return fHeaders.at(i).Header;
}

bool ParamHeaderHelper::HaveHeader(paramId_t i) const {
  auto const &h_it = fHeaders.find(i);
  if (h_it == fHeaders.end()) {
    return false;
  }
  return true;
}

SystParamHeader const &
ParamHeaderHelper::GetHeader(std::string const &name) const {
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!HaveHeader(name)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter named, " << name
                  << ", but it is not currently configured." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return nullheader;
    }
  }
  return fHeaders.at(GetHeaderId(name)).Header;
}
bool ParamHeaderHelper::HaveHeader(std::string const &name) const {
  return (GetHeaderId(name) != kParamUnhandled<paramId_t>);
}
paramId_t ParamHeaderHelper::GetHeaderId(std::string const &name) const {
  for (auto hdr_it : fHeaders) {
    if (hdr_it.second.Header.prettyName == name) {
      return hdr_it.second.Header.systParamId;
    }
  }
  return kParamUnhandled<paramId_t>;
}

param_list_t ParamHeaderHelper::GetParameters() const {
  param_list_t paramIds;
  for (auto const &hdr_it : fHeaders) {
    paramIds.push_back(hdr_it.second.Header.systParamId);
  }
  return paramIds;
}

bool ParamHeaderHelper::IsThrownParam(paramId_t i) const {
  return GetHeader(i).isRandomlyThrown;
}

bool ParamHeaderHelper::IsResponselessParam(paramId_t i) const {
  return GetHeader(i).isResponselessParam;
}
paramId_t ParamHeaderHelper::GetResponseParamId(paramId_t i) const {
  SystParamHeader const &hdr = GetHeader(i);
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!IsResponselessParam(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested response parameter Id for parameter " << i
                  << ", but it is not a responseless parameter." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return kParamUnhandled<paramId_t>;
    }
  }
  return hdr.responseParamId;
}

bool ParamHeaderHelper::IsSplineParam(paramId_t i) const {
  return GetHeader(i).isSplineable;
}

bool ParamHeaderHelper::ValuesAreInNaturalUnits(paramId_t i) const {
  return GetHeader(i).unitsAreNatural;
}

bool ParamHeaderHelper::IsWeightResponse(paramId_t i) const {
  return GetHeader(i).isWeightSystematicVariation;
}

bool ParamHeaderHelper::HasParameterLimits(paramId_t i) const {
  return HasParameterLowLimit(i) || HasParameterUpLimit(i);
}
bool ParamHeaderHelper::HasParameterLowLimit(paramId_t i) const {
  SystParamHeader const &hdr = GetHeader(i);
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!IsSplineParam(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested parameter range lower limit for parameter "
                  << i << ", but it is not a splineable parameter."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return false;
    }
  }
  return hdr.paramValidityRange[0] != kDefaultDouble;
}
bool ParamHeaderHelper::HasParameterUpLimit(paramId_t i) const {
  SystParamHeader const &hdr = GetHeader(i);
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!IsSplineParam(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested parameter range upper limit for parameter "
                  << i << ", but it is not a splineable parameter."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return false;
    }
  }
  return hdr.paramValidityRange[1] != kDefaultDouble;
}
double ParamHeaderHelper::GetParameterLowLimit(paramId_t i) const {
  SystParamHeader const &hdr = GetHeader(i);
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!HasParameterLowLimit(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested parameter range lower limit for parameter "
                  << i << ", but it is not a splineable parameter."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return kDefaultDouble;
    }
  }
  return hdr.paramValidityRange[0];
}
double ParamHeaderHelper::GetParameterUpLimit(paramId_t i) const {
  SystParamHeader const &hdr = GetHeader(i);
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!HasParameterUpLimit(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested parameter range upper limit for parameter "
                  << i << ", but it is not a splineable parameter."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return kDefaultDouble;
    }
  }
  return hdr.paramValidityRange[1];
}

param_value_list_t
ParamHeaderHelper::CheckParamValueList(param_value_list_t ivlist) const {
  for (param_value_list_t::iterator iv_it = ivlist.begin();
       iv_it != ivlist.end();) {
    if (!HaveHeader(iv_it->pid)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << iv_it->pid
                  << ", but it is not currently configured." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      iv_it = ivlist.erase(iv_it);
      continue;
    }
    if (fChkErr.fCare == ParamValidationAndErrorResponse::kTortoise) {
      // If not too fussed about using default behavior, remove the offending
      // parameter.
      if (!IsWeightResponse(iv_it->pid)) {
        if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
          std::cout << "["
                    << ((fChkErr.fPedantry ==
                         ParamValidationAndErrorResponse::kNotOnMyWatch)
                            ? "ERROR"
                            : "WARN")
                    << "]: Requested total response for a map of "
                       "parameter-value pairs, but parameter "
                    << iv_it->pid << ", " << GetHeader(iv_it->pid).prettyName
                    << " is not a weight systematic. " << std::endl;
          if (fChkErr.fPedantry ==
              ParamValidationAndErrorResponse::kNotOnMyWatch) {
            throw;
          }
        }
        iv_it = ivlist.erase(iv_it);
        continue;
      }
    }
    // If not too fussed about using default behavior, remove the offending
    // parameter.
    if (!IsSplineParam(iv_it->pid)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested response for a map of "
                     "parameter-value pairs, but parameter "
                  << iv_it->pid << ", " << GetHeader(iv_it->pid).prettyName
                  << " is not a splineable parameter. " << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      iv_it = ivlist.erase(iv_it);
      continue;
    }
    ++iv_it;
  }
  return ivlist;
}
param_list_t
ParamHeaderHelper::CheckParamList(param_list_t ilist, bool ExpectSpline,
                                  bool RequireWeightResponse) const {
  for (param_list_t::iterator i_it = ilist.begin(); i_it != ilist.end();) {
    if (!HaveHeader(*i_it)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << (*i_it)
                  << ", but it is not currently configured." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      i_it = ilist.erase(i_it);
      continue;
    }
    if (fChkErr.fCare == ParamValidationAndErrorResponse::kTortoise) {
      // If not too fussed about using default behavior, remove the offending
      // parameter.
      if (RequireWeightResponse && !IsWeightResponse(*i_it)) {
        if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
          std::cout << "["
                    << ((fChkErr.fPedantry ==
                         ParamValidationAndErrorResponse::kNotOnMyWatch)
                            ? "ERROR"
                            : "WARN")
                    << "]: Requested total response for a list of parameters, "
                       "but parameter "
                    << *i_it << ", " << GetHeader(*i_it).prettyName
                    << " is not a weight systematic. " << std::endl;
          if (fChkErr.fPedantry ==
              ParamValidationAndErrorResponse::kNotOnMyWatch) {
            throw;
          }
        }
        i_it = ilist.erase(i_it);
        continue;
      }
    }
    // If not too fussed about using default behavior, remove the offending
    // parameter.
    if (ExpectSpline && !IsSplineParam(*i_it)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested spline response for a list of parameters, "
                     "but parameter "
                  << *i_it << ", " << GetHeader(*i_it).prettyName
                  << " is not a splineable systematic. " << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      i_it = ilist.erase(i_it);
      continue;
    }
    // If not too fussed about using default behavior, remove the offending
    // parameter.
    if (IsResponselessParam(*i_it)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested response for a list of parameters, "
                     "but parameter "
                  << *i_it << ", " << GetHeader(*i_it).prettyName
                  << " is a responseless parameter. " << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      i_it = ilist.erase(i_it);
      continue;
    }
    ++i_it;
  }
  return ilist;
}

TSpline3 ParamHeaderHelper::GetSpline(paramId_t i,
                                      spline_t const &event_responses,
                                      SystParamHeader const &hdr) const {

  // Check if the response header suggests that this is a spline-type parameter.
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!IsSplineParam(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested spline for parameter " << i
                  << ", but it is not a splineable parameter." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return TSpline3();
    }
  }

  // Slow, inefficient checks
  if (fChkErr.fCare == ParamValidationAndErrorResponse::kTortoise) {
    scratch_spline_t1 =
        hdr.differsEventByEvent ? event_responses : hdr.responses;
    size_t NResponses = scratch_spline_t1.size();

    // Check if the number of responses found is the same as the number of knots
    if ((NResponses != hdr.paramVariations.size())) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested spline for parameter " << i
                  << ", but the "
                     "number of responses ("
                  << NResponses << ") and knots (" << hdr.paramVariations.size()
                  << ") differ." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      NResponses = std::min(NResponses, hdr.paramVariations.size());
    }

    for (size_t sp_it = 0; sp_it < NResponses; ++sp_it) {
      // Check if any spline responses are outside the limits set, apply set
      // behavior is bad responses are found.
      scratch_spline_t1[sp_it] =
          fChkErr.CheckResponse(scratch_spline_t1[sp_it], hdr, sp_it);
    }

    scratch_spline_t2 = hdr.paramVariations;
    /// No TSpline3 constructor that takes const arrays...
    return TSpline3("", scratch_spline_t2.data(), scratch_spline_t1.data(),
                    NResponses);
  }

  scratch_spline_t2 = hdr.paramVariations;
  scratch_spline_t1 = hdr.differsEventByEvent ? event_responses : hdr.responses;

#ifdef DEBUG_PARAMHEADERHELPER
  std::cout << "[INFO]: Building spline for parameter " << hdr.systParamId
            << ", " << hdr.prettyName << " from " << scratch_spline_t2.size()
            << ", shift values and " << scratch_spline_t1.size()
            << " responses (isGlobal ? " << !hdr.differsEventByEvent << ")."
            << std::endl;
#endif

  return TSpline3("", scratch_spline_t2.data(), scratch_spline_t1.data(),
                  scratch_spline_t2.size());
}
TSpline3 ParamHeaderHelper::GetSpline(paramId_t i,
                                      event_unit_response_t const &eur,
                                      SystParamHeader const &) const {

  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!ContainterHasParam(eur, i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << i
                  << ", but the relevant event response was not passed."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return TSpline3();
    }
  }

  return GetSpline(i, GetParamElementFromContainer(eur, i).responses);
}

TSpline3 ParamHeaderHelper::GetSpline(paramId_t i,
                                      spline_t const &event_responses) const {
  SystParamHeader const &hdr = GetHeader(i);
  return GetSpline(i, event_responses, hdr);
}
TSpline3 ParamHeaderHelper::GetSpline(paramId_t i,
                                      event_unit_response_t const &eur) const {
  SystParamHeader const &hdr = GetHeader(i);
  return GetSpline(i, eur, hdr);
}
std::vector<TSpline3>
ParamHeaderHelper::GetSplines(paramId_t i, EventResponse const &er) const {
  SystParamHeader const &hdr = GetHeader(i);
  std::vector<TSpline3> rtn;
  for (auto &eur : er) {
    rtn.emplace_back(GetSpline(i, eur, hdr));
  }
  return rtn;
}

ParamHeaderHelper::param_tspline_map_t
ParamHeaderHelper::GetSplines(param_list_t const &ilist,
                              event_unit_response_t const &eur) const {
  param_tspline_map_t rtn;
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    param_list_t ilist_cpy = CheckParamList(ilist, true, false);
    for (auto &i : ilist_cpy) {
      // Use this form to allow for lazy handing off of checking.
      rtn[i] = GetSpline(i, eur);
    }
    return rtn;
  }
  for (auto &i : ilist) {
    // Use this form to allow for lazy handing off of checking.
    rtn[i] = GetSpline(i, eur);
  }
  return rtn;
}
std::vector<ParamHeaderHelper::param_tspline_map_t>
ParamHeaderHelper::GetSplines(param_list_t const &ilist,
                              EventResponse const &er) const {

  std::vector<param_tspline_map_t> rtn;
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    param_list_t ilist_cpy = CheckParamList(ilist, true, false);
    for (auto &eu : er) {
      rtn.emplace_back(GetSplines(ilist_cpy, eu));
    }
    return rtn;
  }
  for (auto &eu : er) {
    rtn.emplace_back(GetSplines(ilist, eu));
  }
  return rtn;
}

double
ParamHeaderHelper::GetParameterResponse(paramId_t i, double v,
                                        spline_t const &event_responses) const {
  if (fChkErr.fCare == ParamValidationAndErrorResponse::kHare) {
    if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
      std::cout << "["
                << ((fChkErr.fPedantry ==
                     ParamValidationAndErrorResponse::kNotOnMyWatch)
                        ? "ERROR"
                        : "WARN")
                << "]: The GetParameterResponse interface is extremely "
                   "inefficienct unless you only ever need to evaluate the "
                   "parameter response for a given event once per execution."
                << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
    }
  }

  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!HaveHeader(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << i
                  << ", but it is not currently configured." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return 0;
    }
  }

  // check parameter limits
  if (fChkErr.fCare == ParamValidationAndErrorResponse::kTortoise) {
    if (HasParameterLowLimit(i) && (v < GetParameterLowLimit(i))) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Parameter \"" << GetHeader(i).prettyName
                  << "\", evaluated at " << v
                  << ", but specified lower bound at "
                  << GetParameterLowLimit(i) << "." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }

      v = GetParameterLowLimit(i);
    }
    if (HasParameterUpLimit(i) && (v > GetParameterUpLimit(i))) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Parameter \"" << GetHeader(i).prettyName
                  << "\", evaluated at " << v
                  << ", but specified upper bound at " << GetParameterUpLimit(i)
                  << "." << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }

      v = GetParameterUpLimit(i);
    }
  }
  return GetSpline(i, event_responses).Eval(v);
}

double ParamHeaderHelper::GetParameterResponse(
    paramId_t i, double v, event_unit_response_t const &eur) const {

  // Manually do this check here (from GetSpline) as it seems to be the path of
  // least duplication.
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!ContainterHasParam(eur, i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << i
                  << ", but the relevant event response was not passed."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
        // If for some reason we have the header info and it is a weight header,
        // respect the error response settings.
        if (IsWeightResponse(i)) {
          return fChkErr.fErrorResponse ==
                         ParamValidationAndErrorResponse::kUnityWeight
                     ? 1
                     : 0;
        } else { // otherwise just reply with a 0.
          return 0;
        }
      }
    }
  }

  return GetParameterResponse(i, v,
                              GetParamElementFromContainer(eur, i).responses);
}

double
ParamHeaderHelper::GetTotalResponse(param_value_list_t const &ivlist,
                                    event_unit_response_t const &eur) const {

  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    param_value_list_t ivmap_cpy = CheckParamValueList(ivlist);
    double response_weight = 1;
    for (auto &iv : ivmap_cpy) {
      // Use this form to allow for lazy handing off of checking.
      response_weight *= GetParameterResponse(iv.pid, iv.val, eur);
    }
    return response_weight;
  }

  double response_weight = 1;
  for (auto &iv : ivlist) {
    // Use this form to allow for lazy handing off of checking.
    response_weight *= GetParameterResponse(iv.pid, iv.val, eur);
  }
  return response_weight;
}

std::vector<double>
ParamHeaderHelper::GetParameterResponse(paramId_t i, double v,
                                        EventResponse const &er) const {
  std::vector<double> rtn;
  for (auto &eur : er) {
    rtn.push_back(GetParameterResponse(i, v, eur));
  }
  return rtn;
}
std::vector<double>
ParamHeaderHelper::GetTotalResponse(param_value_list_t const &ivlist,
                                    EventResponse const &er) const {
  std::vector<double> rtn;
  for (auto &eur : er) {
    rtn.push_back(GetTotalResponse(ivlist, eur));
  }
  return rtn;
}

size_t ParamHeaderHelper::GetNDiscreteVariations(paramId_t i) const {
  SystParamHeader const &hdr = GetHeader(i);
  return hdr.paramVariations.size();
}

std::vector<size_t>
ParamHeaderHelper::GetNDiscreteVariations(param_list_t const &paramlist) const {
  std::vector<size_t> rtn;
  for (auto &i : paramlist) {
    rtn.push_back(GetNDiscreteVariations(i));
  }
  return rtn;
}

ParamHeaderHelper::discrete_variation_list_t
ParamHeaderHelper::GetDiscreteResponses(
    paramId_t i, discrete_variation_list_t const &event_responses,
    SystParamHeader const &hdr) const {

  // Check if the response header suggests that this is a responseless
  // parameter.
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (IsResponselessParam(i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested responses for parameter " << i
                  << ", but it expresses responses through parameter "
                  << GetResponseParamId(i) << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return discrete_variation_list_t{};
    }

    // Slow, inefficient checks
    if (fChkErr.fCare == ParamValidationAndErrorResponse::kTortoise) {

      discrete_variation_list_t scratch_discrete_variation_list_t1 =
          hdr.differsEventByEvent ? event_responses : hdr.responses;
      size_t NResponses = scratch_discrete_variation_list_t1.size();

      // Check if the number of responses found is the same as the number of
      // knots
      if ((NResponses != hdr.paramVariations.size())) {
        if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
          std::cout << "["
                    << ((fChkErr.fPedantry ==
                         ParamValidationAndErrorResponse::kNotOnMyWatch)
                            ? "ERROR"
                            : "WARN")
                    << "]: Requested discrete variations for parameter " << i
                    << ", but the number of responses (" << NResponses
                    << ") and variations (" << hdr.paramVariations.size()
                    << ") differ." << std::endl;
          if (fChkErr.fPedantry ==
              ParamValidationAndErrorResponse::kNotOnMyWatch) {
            throw;
          }
        }
        NResponses = std::min(NResponses, hdr.paramVariations.size());
      }

      for (size_t sp_it = 0; sp_it < NResponses; ++sp_it) {
        // Check if any responses are outside the limits set, apply set behavior
        // is bad responses are found.
        scratch_discrete_variation_list_t1[sp_it] = fChkErr.CheckResponse(
            scratch_discrete_variation_list_t1[sp_it], hdr, sp_it);
      }
      scratch_discrete_variation_list_t1.resize(NResponses);
      return scratch_discrete_variation_list_t1;
    }
  }

  return hdr.differsEventByEvent ? event_responses : hdr.responses;
}

ParamHeaderHelper::discrete_variation_list_t
ParamHeaderHelper::GetDiscreteResponses(
    paramId_t i, discrete_variation_list_t const &event_responses) const {
  return GetDiscreteResponses(i, event_responses, GetHeader(i));
}

ParamHeaderHelper::discrete_variation_list_t
ParamHeaderHelper::GetDiscreteResponses(paramId_t i,
                                        event_unit_response_t const &eur,
                                        SystParamHeader const &hdr) const {

  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!ContainterHasParam(eur, i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << i
                  << ", but the relevant event response was not passed."
                  << std::endl;
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
      }
      return {};
    }
  }

  return ParamHeaderHelper::GetDiscreteResponses(
      i, GetParamElementFromContainer(eur, i).responses, hdr);
}
ParamHeaderHelper::discrete_variation_list_t
ParamHeaderHelper::GetDiscreteResponses(
    paramId_t i, event_unit_response_t const &eur) const {
  return GetDiscreteResponses(i, eur, GetHeader(i));
}

double ParamHeaderHelper::GetDiscreteResponse(
    paramId_t i, size_t j,
    discrete_variation_list_t const &event_responses) const {

  if (fChkErr.fCare == ParamValidationAndErrorResponse::kHare) {
    if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
      std::cout
          << "["
          << ((fChkErr.fPedantry ==
               ParamValidationAndErrorResponse::kNotOnMyWatch)
                  ? "ERROR"
                  : "WARN")
          << "]: The GetDiscreteResponse interface is extremely "
             "inefficienct. The advised practice is to use "
             "GetDiscreteResponses to get all variations for a given set of "
             "events and then use or cache the results."
          << std::endl;
      if (fChkErr.fPedantry == ParamValidationAndErrorResponse::kNotOnMyWatch) {
        throw;
      }
    }
  }

  return GetDiscreteResponses(i, event_responses)[j];
}

double
ParamHeaderHelper::GetDiscreteResponse(paramId_t i, size_t j,
                                       event_unit_response_t const &eur) const {

  // Manually do this check here (from GetSpline) as it seems to be the path of
  // least duplication.
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    if (!ContainterHasParam(eur, i)) {
      if (fChkErr.fPedantry <= ParamValidationAndErrorResponse::kMeh) {
        std::cout << "["
                  << ((fChkErr.fPedantry ==
                       ParamValidationAndErrorResponse::kNotOnMyWatch)
                          ? "ERROR"
                          : "WARN")
                  << "]: Requested header for parameter " << i
                  << ", but the relevant event response was not passed."
                  << std::endl;
        std::cout << "[INFO]: Recieved response info for: " << std::endl;
        for (auto &ivs : eur) {
          std::cout << "\t" << ivs.pid << std::endl;
        }
        if (fChkErr.fPedantry ==
            ParamValidationAndErrorResponse::kNotOnMyWatch) {
          throw;
        }
        // If for some reason we have the header info and it is a weight header,
        // respect the error response settings.
        if (IsWeightResponse(i)) {
          return fChkErr.fErrorResponse ==
                         ParamValidationAndErrorResponse::kUnityWeight
                     ? 1
                     : 0;
        } else { // otherwise just reply with a 0.
          return 0;
        }
      }
    }
  }

  return GetDiscreteResponse(i, j,
                             GetParamElementFromContainer(eur, i).responses);
}

double
ParamHeaderHelper::GetDiscreteResponse(param_list_t const &ilist, size_t j,
                                       event_unit_response_t const &eur) const {

  double response_weight = 1;
  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    param_list_t ilist_cpy = CheckParamList(ilist, false, true);
    for (auto &i : ilist_cpy) {
      // Use this form to allow for lazy handing off of checking.
      response_weight *= GetDiscreteResponse(i, j, eur);
    }
    return response_weight;
  }

  for (auto &i : ilist) {
    // Use this for to get lazy checking of whether the throws are in eur
    response_weight *= GetDiscreteResponse(i, j, eur);
  }
  return response_weight;
}

std::vector<double>
ParamHeaderHelper::GetDiscreteResponses(paramId_t i, size_t j,
                                        EventResponse const &er) const {
  std::vector<double> rtn;
  for (auto &eur : er) {
    // Use this for to get lazy checking of whether the throws are in eur
    rtn.push_back(GetDiscreteResponse(i, j, eur));
  }
  return rtn;
}

std::vector<double>
ParamHeaderHelper::GetDiscreteResponses(param_list_t const &ilist, size_t j,
                                        EventResponse const &er) const {
  std::vector<double> rtn;
  for (auto &eur : er) {
    rtn.push_back(GetDiscreteResponse(ilist, j, eur));
  }
  return rtn;
}

std::vector<ParamHeaderHelper::discrete_variation_list_t>
ParamHeaderHelper::GetAllDiscreteResponses(paramId_t i,
                                           EventResponse const &er) const {
  std::vector<std::vector<double>> rtn;
  for (auto &eur : er) {
    rtn.emplace_back(GetDiscreteResponses(i, eur));
  }
  return rtn;
}

std::vector<ParamHeaderHelper::discrete_variation_list_t>
ParamHeaderHelper::GetAllDiscreteResponses(param_list_t const &ilist,
                                           EventResponse const &er) const {

  size_t nvariations = GetNDiscreteVariations(ilist.front());
  std::vector<std::vector<double>> rtn;

  if (fChkErr.fCare <= ParamValidationAndErrorResponse::kFrog) {
    param_list_t ilist_cpy = CheckParamList(ilist, false, true);
    for (auto &eur : er) {
      rtn.emplace_back(nvariations, 1);
      for (auto &i : ilist_cpy) {
        std::vector<double> const &responses = GetDiscreteResponses(i, eur);
        for (size_t t = 0; t < nvariations; ++t) {
          rtn.back()[t] *= responses[t];
        }
      }
    }
    return rtn;
  }

  for (auto &eur : er) {
    rtn.emplace_back(nvariations, 1);
    for (auto &i : ilist) {
      std::vector<double> const &responses = GetDiscreteResponses(i, eur);
      for (size_t t = 0; t < nvariations; ++t) {
        rtn.back()[t] *= responses[t];
      }
    }
  }
  return rtn;
}

std::map<paramId_t, ParamHeaderHelper::discrete_variation_list_t>
ParamHeaderHelper::GetDiscreteVariationParameterValues(
    param_list_t const &ilist) const {
  std::map<paramId_t, discrete_variation_list_t> discrete_var_param_values;
  for (auto const &i : ilist) {
    discrete_var_param_values[i] = GetHeader(i).paramVariations;
  }
  return discrete_var_param_values;
}

std::string ParamHeaderHelper::GetHeaderInfo() const {
  std::stringstream ss("");

  for (paramId_t p : GetParameters()) {
    SystParamHeader const &hdr = GetHeader(p);
    ss << " Parameter: { \n\tId: " << hdr.systParamId
       << ", \n\tname: " << hdr.prettyName
       << ", \n\tNParamValues: " << hdr.paramVariations.size() << " }."
       << std::endl;
  }
  return ss.str();
}
std::string
ParamHeaderHelper::GetEventResponseInfo(event_unit_response_t eur) const {
  std::stringstream ss("");

  for (paramId_t p : GetParameters()) {
    SystParamHeader const &hdr = GetHeader(p);
    ss << " Parameter: { \n\tId: " << hdr.systParamId
       << ", \n\tname: " << hdr.prettyName
       << ", \n\tNParamValues: " << hdr.paramVariations.size() << std::flush;

    if (!ContainterHasParam(eur, p)) {
      ss << " }." << std::endl;
      continue;
    }

    if (hdr.isCorrection) {
      ss << ", \n\tcorrection: (" << hdr.centralParamValue << " -> "
         << GetParamElementFromContainer(eur, p).responses[0] << ") }."
         << std::endl;
      continue;
    }

    ss << ", \n\t(val -> resp): [ " << std::endl;

    auto const &responses = GetParamElementFromContainer(eur, p).responses;
    size_t NResponses = responses.size();
    for (size_t i = 0; i < hdr.paramVariations.size(); ++i) {
      ss << "\t                 (" << hdr.paramVariations[i] << " -> "
         << ((NResponses > i) ? std::to_string(responses[i]) : "NR")
         << ((i + 1 == hdr.paramVariations.size()) ? ")" : "),") << std::endl;
    }
    ss << "\t]\n }." << std::endl;
  }
  return ss.str();
}

} // namespace systtools
