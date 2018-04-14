#include "larsyst/interface/EventResponse_product.hxx"
#include "larsyst/interface/SystMetaData_product.hxx"

#include "canvas/Persistency/Common/Wrapper.h"

#include <vector>

template class art::Wrapper<larsyst::SystParamHeader>;
template class art::Wrapper<larsyst::SystMetaData>;
template class art::Wrapper<larsyst::EventResponse>;
