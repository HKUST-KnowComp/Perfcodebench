#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0) return 0;

  // Decoded nibbles are in [0, 15]. If threshold >= 15, nothing can pass.
  if (threshold >= 15) return 0;

  const uint8_t t = threshold;
  const std::size_t n = packed.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    const uint8_t* p = packed.data();
    const uint8_t* end = p + n;

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
