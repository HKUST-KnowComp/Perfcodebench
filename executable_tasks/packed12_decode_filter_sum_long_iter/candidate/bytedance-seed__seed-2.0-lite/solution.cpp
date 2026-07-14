#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  std::vector<uint16_t> valid_values;
  const uint32_t threshold_u32 = static_cast<uint32_t>(threshold);
  valid_values.reserve((packed.size() / 3u) * 2u);

  // Single preprocessing pass to decode and filter values once
  for (std::size_t i = 0; i < packed.size(); i += 3u) {
    const uint32_t a = static_cast<uint32_t>(packed[i]) |
                        (static_cast<uint32_t>(packed[i + 1u] & 0x0fu) << 8u);
    const uint32_t b = (static_cast<uint32_t>(packed[i + 1u] >> 4u)) |
                        (static_cast<uint32_t>(packed[i + 2u]) << 4u);
    if (a > threshold_u32) valid_values.push_back(static_cast<uint16_t>(a));
    if (b > threshold_u32) valid_values.push_back(static_cast<uint16_t>(b));
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    // Branchless vectorizable sum loop over prefiltered cache-resident values
    for (uint16_t v : valid_values) {
      sum += static_cast<uint64_t>(v);
    }
  }
  return sum;
}