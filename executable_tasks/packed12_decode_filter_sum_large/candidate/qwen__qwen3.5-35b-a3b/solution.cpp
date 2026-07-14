#pragma once

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t total_sum = 0;
  const std::size_t num_values = (packed.size() / 3u) * 2u;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < packed.size(); i += 3u) {
      const uint32_t a =
          static_cast<uint32_t>(packed[i]) |
          (static_cast<uint32_t>(packed[i + 1u] & 0x0fu) << 8u);
      const uint32_t b =
          (static_cast<uint32_t>(packed[i + 1u] >> 4u)) |
          (static_cast<uint32_t>(packed[i + 2u]) << 4u);
      
      if (a > threshold) current_sum += a;
      if (b > threshold) current_sum += b;
    }
    total_sum += current_sum;
  }
  
  return total_sum;
}