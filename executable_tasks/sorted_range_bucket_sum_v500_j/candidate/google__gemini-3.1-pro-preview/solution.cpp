#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized range_bucket_sum implementation.
 * 
 * The original implementation used std::upper_bound for every probe, resulting in O(iters * M * log N) complexity.
 * Since both 'boundaries' and 'probes' are sorted, we can use a two-pointer approach to achieve O(iters * (M + N)).
 * This is significantly faster when M and N are large.
 */
uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (probes.empty()) {
    return 0;
  }

  const uint32_t* const b_start = boundaries.data();
  const uint32_t* const b_end = b_start + boundaries.size();
  const uint32_t* const p_start = probes.data();
  const uint32_t* const p_end = p_start + probes.size();

  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    const uint32_t* b_curr = b_start;
    
    // Two-pointer linear scan
    for (const uint32_t* p_curr = p_start; p_curr < p_end; ++p_curr) {
      const uint32_t p_val = *p_curr;
      
      // Advance boundary pointer as long as boundary value is <= probe value
      // This finds the first boundary > probe, which is the definition of upper_bound
      while (b_curr < b_end && *b_curr <= p_val) {
        b_curr++;
      }
      
      // The index is the distance from the start
      current_sum += static_cast<uint64_t>(b_curr - b_start);
    }
    last_sum = current_sum;
  }

  return last_sum;
}
