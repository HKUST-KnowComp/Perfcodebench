#pragma once

#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    size_t b_size = boundaries.size();
    for (uint32_t probe : probes) {
      while (b_idx < b_size && boundaries[b_idx] <= probe) {
        b_idx++;
      }
      sum += static_cast<uint64_t>(b_idx);
    }
  }
  return sum;
}