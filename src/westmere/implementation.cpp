#include "westmere/begin_implementation.h"
#include "westmere/dom_parser_implementation.h"

namespace SIMDJSON_IMPLEMENTATION {
namespace {

SIMDJSON_WARN_UNUSED error_code implementation::create_dom_parser_implementation(
  size_t capacity,
  size_t max_depth,
  std::unique_ptr<internal::dom_parser_implementation>& dst
) const noexcept {
  dst.reset( new (std::nothrow) dom_parser_implementation() );
  if (!dst) { return MEMALLOC; }
  dst->set_capacity(capacity);
  dst->set_max_depth(max_depth);
  return SUCCESS;
}

} // unnamed namespace
} // namespace SIMDJSON_IMPLEMENTATION

#include "westmere/end_implementation.h"