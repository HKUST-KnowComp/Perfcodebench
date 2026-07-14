#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  size_t b_idx = 0;
  const size_t b_size = boundaries.size();
  for (uint32_t probe : probes) {
    while (b_idx < b_size && boundaries[b_idx] <= probe) {
      ++b_idx;
    }
    sum += b_idx;
  }
  return sum;
}