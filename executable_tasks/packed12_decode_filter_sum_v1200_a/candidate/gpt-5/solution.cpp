#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  // Preserve baseline behavior when iters == 0
  if (iters <= 0) {
    return 0;
  }

  // Early exits for trivial cases
  if (packed.empty()) {
    return 0;
  }

  const uint32_t T = static_cast<uint32_t>(threshold);
  // 12-bit max value is 4095; if threshold >= 4095, nothing can be greater
  if (T >= 0x0FFFu) {
    return 0;
  }

  const uint8_t* p = packed.data();
  const std::size_t ntriples = packed.size() / 3u; // input is assumed well-formed

  uint64_t sum = 0;
  for (std::size_t t = 0; t < ntriples; ++t) {
    const uint32_t b0 = static_cast<uint32_t>(p[0]);
    const uint32_t b1 = static_cast<uint32_t>(p[1]);
    const uint32_t b2 = static_cast<uint32_t>(p[2]);

    const uint32_t a = b0 | ((b1 & 0x0Fu) << 8u);
    const uint32_t b = (b1 >> 4u) | (b2 << 4u);

    if (a > T) sum += static_cast<uint64_t>(a);
    if (b > T) sum += static_cast<uint64_t>(b);

    p += 3;
  }

  return sum;
}
