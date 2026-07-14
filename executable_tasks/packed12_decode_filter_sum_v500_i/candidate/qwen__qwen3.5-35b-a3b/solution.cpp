#pragma once

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t num_bytes = packed.size();
  const std::size_t num_values = (num_bytes / 3u) * 2u;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < num_bytes; i += 3u) {
      const uint32_t b0 = packed[i];
      const uint32_t b1 = packed[i + 1u];
      const uint32_t b2 = packed[i + 2u];

      const uint32_t a = b0 | ((b1 & 0x0fu) << 8u);
      const uint32_t b = ((b1 >> 4u) & 0x0fu) | (b2 << 4u);

      if (a > threshold) current_sum += a;
      if (b > threshold) current_sum += b;
    }
    sum = current_sum;
  }

  return sum;
}