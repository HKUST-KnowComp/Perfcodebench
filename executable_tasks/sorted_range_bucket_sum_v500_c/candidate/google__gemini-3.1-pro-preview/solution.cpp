#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized sorted range bucketing.
 * 
 * Given that both 'boundaries' and 'probes' are sorted, we can replace the 
 * O(log N) binary search per probe with a two-pointer approach that runs in O(N + M).
 * This is significantly more cache-friendly and avoids redundant comparisons.
 */
uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const size_t num_boundaries = boundaries.size();
  const size_t num_probes = probes.size();
  const uint32_t* b_ptr = boundaries.data();
  const uint32_t* p_ptr = probes.data();

  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t b_idx = 0;

    // Two-pointer approach: since probes are sorted, the boundary index 
    // for probe[i+1] must be >= the boundary index for probe[i].
    for (size_t p_idx = 0; p_idx < num_probes; ++p_idx) {
      const uint32_t p_val = p_ptr[p_idx];
      
      // Advance the boundary pointer as long as the boundary value is <= probe value.
      // This finds the first boundary > probe (std::upper_bound logic).
      while (b_idx < num_boundaries && b_ptr[b_idx] <= p_val) {
        b_idx++;
      }
      
      // b_idx is now the count of elements in 'boundaries' that are <= p_val,
      // which is exactly the index returned by std::upper_bound - begin().
      current_sum += static_cast<uint64_t>(b_idx);
    }
    last_sum = current_sum;
  }

  return last_sum;
}