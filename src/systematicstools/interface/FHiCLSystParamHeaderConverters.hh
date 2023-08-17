#pragma once

#include "systematicstools/utility/exceptions.hh"
#include "fhiclcpp/fwd.h"

#include <string>

namespace systtools {
struct SystParamHeader;
}

namespace systtools {

/// Exception thrown when an unexpected key is found in a fhicl::ParameterSet
/// being parsed as a SystParamHeader
NEW_SYSTTOOLS_EXCEPT(invalid_SystParamHeader_key);

///\brief Deserializes a SystParamHeader instance from a passed FHiCL parameter
/// set.
SystParamHeader FHiCLToSystParamHeader(fhicl::ParameterSet const &paramset);

///\brief Serializes a SyhstParamHeader instance to a FHiCL table.
fhicl::ParameterSet SystParamHeaderToFHiCL(SystParamHeader const &sph);
}
