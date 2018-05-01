#ifndef LARSYST_INTERPRETERS_VALIDATESYSTMETADATA_SEEN
#define LARSYST_INTERPRETERS_VALIDATESYSTMETADATA_SEEN

#include "larsyst/interface/SystMetaData.hh"

namespace larsyst {
bool validate_SystMetaData(SystMetaData const &, bool quiet=true);
}

#endif
