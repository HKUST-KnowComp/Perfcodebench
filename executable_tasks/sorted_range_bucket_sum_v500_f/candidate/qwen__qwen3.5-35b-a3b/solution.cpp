#pragma once

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t boundary_idx = 0;
    for (uint32_t probe : probes) {
      while (boundary_idx < boundaries.size() && boundaries[boundary_idx] <= probe) {
        boundary_idx++;
      }
      sum += boundary_idx;
    }
  }
  return sum;
}