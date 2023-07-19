#pragma once

#include "systematicstools/utility/exceptions.hh"

#include <string>

namespace fhiclsimple {
class ParameterSet;
}
namespace systtools {
struct SystParamHeader;
}

namespace systtools {

/// Exception thrown when an unexpected key is found in a fhiclsimple::ParameterSet
/// being parsed as a SystParamHeader
NEW_SYSTTOOLS_EXCEPT(invalid_SystParamHeader_key);

///\brief Deserializes a SystParamHeader instance from a passed FHiCL parameter
/// set.
SystParamHeader FHiCLToSystParamHeader(fhiclsimple::ParameterSet const &paramset);

///\brief Serializes a SyhstParamHeader instance to a FHiCL table.
fhiclsimple::ParameterSet SystParamHeaderToFHiCL(SystParamHeader const &sph);
}
