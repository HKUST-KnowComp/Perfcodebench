#pragma once

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  std::vector<uint32_t> decoded(deltas.size());
  uint64_t total_sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < deltas.size(); ++i) {
      uint32_t value = base + static_cast<uint32_t>(deltas[i]);
      if (value >= low && value <= high) {
        iter_sum += value;
      }
    }
    total_sum += iter_sum;
  }
  
  return total_sum;
}