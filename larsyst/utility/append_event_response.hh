#ifndef LARSYST_UTILITY_APPENDEVENTRESPONSE_SEEN
#define LARSYST_UTILITY_APPENDEVENTRESPONSE_SEEN

#include "larsyst/interface/EventResponse_product.hh"

#include <memory>

namespace larsyst {
void append_event_response(std::unique_ptr<larsyst::EventResponse> &,
                            std::unique_ptr<larsyst::EventResponse> &&);
}
#endif
