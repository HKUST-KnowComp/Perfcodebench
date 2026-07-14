#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;
  // Direct lookup table for all possible uint16_t code values.
  std::vector<uint8_t> seen(65536, 0);
  std::vector<uint16_t> seen_codes;
  seen_codes.reserve(codes.size() < 65536 ? codes.size() : 65536);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_count = 0;
    for (uint16_t code : codes) {
      if (!seen[code]) {
        seen[code] = 1;
        seen_codes.push_back(code);
        ++current_count;
      }
    }
    count = current_count;
    // Reset only the entries we touched to avoid O(65536) memset per iteration.
    for (uint16_t c : seen_codes) {
      seen[c] = 0;
    }
    seen_codes.clear();
  }
  (void)dictionary;
  return count;
}
