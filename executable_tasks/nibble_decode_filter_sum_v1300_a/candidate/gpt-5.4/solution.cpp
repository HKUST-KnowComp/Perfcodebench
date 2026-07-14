#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) {
    return 0;
  }

  // Decoded values are 4-bit nibbles in [0, 15].
  // If threshold >= 15, no value can pass the filter.
  if (threshold >= 15) {
    return 0;
  }

  const uint8_t t = threshold;
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    const uint8_t* p = packed.data();
    const uint8_t* end = p + packed.size();

    for (; p != end; ++p) {
      const uint8_t byte = *p;
      const uint8_t lo = static_cast<uint8_t>(byte & 0x0Fu);
      const uint8_t hi = static_cast<uint8_t>(byte >> 4);

      if (lo > t) local_sum += lo;
      if (hi > t) local_sum += hi;
    }

    sum = local_sum;
  }

  return sum;
}
