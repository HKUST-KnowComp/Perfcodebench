#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  // Match baseline behavior: if iters <= 0, return 0 without doing any work
  if (iters <= 0) {
    return 0;
  }

  const uint32_t thr = static_cast<uint32_t>(threshold);
  // 12-bit values are in [0, 4095]. If threshold >= 4095, nothing can be greater.
  if (thr >= 0x0FFFu) {
    return 0;
  }

  const std::size_t groups = packed.size() / 3u; // each group of 3 bytes yields two 12-bit values
  const uint8_t* p = packed.data();

  uint64_t sum = 0;
  for (std::size_t g = 0; g < groups; ++g) {
    const uint32_t b0 = static_cast<uint32_t>(p[0]);
    const uint32_t b1 = static_cast<uint32_t>(p[1]);
    const uint32_t b2 = static_cast<uint32_t>(p[2]);

    const uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
    const uint32_t b = (b1 >> 4) | (b2 << 4);

    // Branchless accumulation to reduce mispredictions
    sum += static_cast<uint64_t>(a) * static_cast<uint64_t>(a > thr);
    sum += static_cast<uint64_t>(b) * static_cast<uint64_t>(b > thr);

    p += 3;
  }

  return sum;
}
