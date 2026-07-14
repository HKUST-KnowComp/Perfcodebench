#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized sorted range bucketing.
 * 
 * The baseline implementation uses std::upper_bound for every probe, resulting in O(iters * M * log N) complexity.
 * Since both 'boundaries' and 'probes' are sorted, we can use a two-pointer approach to achieve O(iters * (N + M)).
 * This approach iterates through both arrays linearly, which is much more cache-friendly and reduces the 
 * total number of comparisons.
 */
uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (probes.empty()) {
    return 0;
  }

  const uint32_t* const b_data = boundaries.data();
  const size_t b_size = boundaries.size();
  const uint32_t* const p_data = probes.data();
  const size_t p_size = probes.size();

  uint64_t last_sum = 0;

  // The task requires returning the sum of bucket IDs for the probes.
  // The baseline resets 'sum = 0' inside the 'iters' loop, effectively returning the result of the last iteration.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t b_idx = 0;

    // Two-pointer linear scan
    for (size_t p_idx = 0; p_idx < p_size; ++p_idx) {
      const uint32_t p = p_data[p_idx];
      
      // Advance boundary pointer until we find the first boundary > probe
      while (b_idx < b_size && b_data[b_idx] <= p) {
        b_idx++;
      }
      
      // The bucket index is the number of boundaries <= probe
      current_sum += static_cast<uint64_t>(b_idx);
    }
    last_sum = current_sum;
  }

  return last_sum;
}
