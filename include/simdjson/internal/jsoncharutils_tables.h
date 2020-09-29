#ifndef SIMDJSON_INTERNAL_JSONCHARUTILS_TABLES_H
#define SIMDJSON_INTERNAL_JSONCHARUTILS_TABLES_H

#include "simdjson.h"

#ifdef JSON_TEST_STRINGS
void found_string(const uint8_t *buf, const uint8_t *parsed_begin,
                  const uint8_t *parsed_end);
void found_bad_string(const uint8_t *buf);
#endif

namespace simdjson {
namespace internal {

constexpr int FASTFLOAT_SMALLEST_POWER = -325;
constexpr int FASTFLOAT_LARGEST_POWER = 308;

struct value128 {
  uint64_t low;
  uint64_t high;
};

// structural chars here are
// they are { 0x7b } 0x7d : 0x3a [ 0x5b ] 0x5d , 0x2c (and NULL)
// we are also interested in the four whitespace characters
// space 0x20, linefeed 0x0a, horizontal tab 0x09 and carriage return 0x0d

extern SIMDJSON_DLLIMPORTEXPORT const bool structural_or_whitespace_negated[256];
extern SIMDJSON_DLLIMPORTEXPORT const bool structural_or_whitespace[256];
extern SIMDJSON_DLLIMPORTEXPORT const uint32_t digit_to_val32[886];

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INTERNAL_JSONCHARUTILS_TABLES_H
