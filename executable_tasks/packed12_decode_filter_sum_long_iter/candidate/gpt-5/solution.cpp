#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int /*iters*/) {
  // Early-out: all decoded 12-bit values are <= 4095
  if (threshold >= 0x0FFFu) {
    return 0ull;
  }

  const std::size_t triplets = packed.size() / 3u;  
  const uint8_t* p = packed.data();
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0ull;

  // Decode two 12-bit values from each 3-byte group and accumulate if > threshold
  for (std::size_t t = 0; t < triplets; ++t) {
    const uint32_t b0 = static_cast<uint32_t>(p[0]);
    const uint32_t b1 = static_cast<uint32_t>(p[1]);
    const uint32_t b2 = static_cast<uint32_t>(p[2]);

    const uint32_t a = b0 | ((b1 & 0x0Fu) << 8);            // low 12 bits
    const uint32_t b = (b1 >> 4) | (b2 << 4);               // next 12 bits

    // Branchless add-if-greater-than-threshold
    const uint64_t maskA = static_cast<uint64_t>(-(int)(a > thr));
    const uint64_t maskB = static_cast<uint64_t>(-(int)(b > thr));
    sum += (static_cast<uint64_t>(a) & maskA);
    sum += (static_cast<uint64_t>(b) & maskB);

    p += 3;
  }

  return sum;
}
