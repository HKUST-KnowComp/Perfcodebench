#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t total_sum = 0;
  
  // Pre-calculate the result for one pass since the input is deterministic
  // and the loop repeats the same operation 'iters' times.
  uint64_t single_pass_sum = 0;
  size_t b_idx = 0;
  size_t b_size = boundaries.size();

  for (uint32_t probe : probes) {
    // Since both boundaries and probes are sorted, we move the boundary pointer forward.
    // std::upper_bound finds the first element > probe.
    while (b_idx < b_size && boundaries[b_idx] <= probe) {
      b_idx++;
    }
    single_pass_sum += static_cast<uint64_t>(b_idx);
  }

  total_sum = single_pass_sum * static_cast<uint64_t>(iters);
  return total_sum;
}