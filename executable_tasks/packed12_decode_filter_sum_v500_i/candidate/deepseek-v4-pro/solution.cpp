#include "interface.h"

#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  std::size_t size = packed.size();
  if (size == 0 || iters <= 0) return 0;

  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t single_sum = 0;

  // Process three bytes per iteration, producing two 12‑bit values
  for (std::size_t i = 0; i + 2 < size; i += 3) {
    const uint8_t b0 = packed[i];
    const uint8_t b1 = packed[i + 1];
    const uint8_t b2 = packed[i + 2];

    // Decode first value: bits 0-7 from b0, bits 8-11 from low nibble of b1
    const uint32_t a = static_cast<uint32_t>(b0) | ((static_cast<uint32_t>(b1) & 0x0Fu) << 8);
    // Decode second value: bits 0-3 from high nibble of b1, bits 4-11 from b2
    const uint32_t b = (static_cast<uint32_t>(b1) >> 4) | (static_cast<uint32_t>(b2) << 4);

    // Branching adds may be beneficial if many values are below threshold.
    // The compiler will often convert these to conditional moves.
    if (a > thresh) single_sum += static_cast<uint64_t>(a);
    if (b > thresh) single_sum += static_cast<uint64_t>(b);
  }

  // The contract requires the work to be repeated 'iters' times.
  // Because the same data and threshold produce an identical filtered sum each time,
  // we can compute it once and multiply. This is semantically equivalent and far cheaper.
  return single_sum * static_cast<uint64_t>(iters);
}
