#pragma once

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t total_sum = 0;
  uint64_t value = base;
  
  for (int iter = 0; iter < iters; ++iter) {
    value = base;
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < deltas.size(); ++i) {
      value += static_cast<uint64_t>(deltas[i]);
      iter_sum += value;
    }
    total_sum += iter_sum;
  }
  
  return total_sum;
}