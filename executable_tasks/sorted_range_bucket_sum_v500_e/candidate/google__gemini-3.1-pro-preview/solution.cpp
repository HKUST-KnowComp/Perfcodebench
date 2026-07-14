#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized range_bucket_sum for sorted boundaries and sorted probes.
 * 
 * Since both 'boundaries' and 'probes' are sorted, we can use a two-pointer
 * approach (linear scan) to find the bucket index for each probe in O(N + M) time.
 * This is significantly faster than the O(M log N) approach using std::upper_bound
 * for each probe individually, especially when M and N are large.
 */
uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (probes.empty()) {
    return 0;
  }

  const uint32_t* const b_start = boundaries.data();
  const size_t b_size = boundaries.size();
  const uint32_t* const p_start = probes.data();
  const size_t p_size = probes.size();

  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t b_idx = 0;

    // Two-pointer approach: 
    // For each probe, advance the boundary index until boundaries[b_idx] > probe.
    // Because probes are sorted, b_idx only moves forward.
    for (size_t p_idx = 0; p_idx < p_size; ++p_idx) {
      const uint32_t p_val = p_start[p_idx];
      
      // Advance b_idx as long as the boundary is less than or equal to the probe.
      // This finds the first boundary > probe, which is the definition of upper_bound.
      while (b_idx < b_size && b_start[b_idx] <= p_val) {
        b_idx++;
      }
      
      current_sum += static_cast<uint64_t>(b_idx);
    }
    last_sum = current_sum;
  }

  return last_sum;
}
