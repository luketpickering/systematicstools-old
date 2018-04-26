#ifndef LARSYST_INTERPRETERS_VALIDATESYSTPARAMHEADER_SEEN
#define LARSYST_INTERPRETERS_VALIDATESYSTPARAMHEADER_SEEN

#include "larsyst/interface/SystMetaData.hh"

namespace larsyst {
bool validate_SystParamHeader(SystParamHeader const &, bool quiet=true);
}

#endif
