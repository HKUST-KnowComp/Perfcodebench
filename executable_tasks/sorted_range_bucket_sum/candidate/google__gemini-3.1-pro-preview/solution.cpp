#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const size_t b_size = boundaries.size();
  const size_t p_size = probes.size();
  
  // Handle empty cases early to simplify the loop logic
  if (p_size == 0) return 0;
  if (b_size == 0) return 0;

  uint64_t final_sum = 0;
  const uint32_t* b_ptr = boundaries.data();
  const uint32_t* p_ptr = probes.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t b_idx = 0;
    
    // Since both boundaries and probes are sorted, we can use a two-pointer approach.
    // For each probe, we advance the boundary index until we find the first boundary 
    // that is strictly greater than the probe value.
    for (size_t i = 0; i < p_size; ++i) {
      uint32_t p_val = p_ptr[i];
      
      // Advance b_idx as long as the current boundary is less than or equal to the probe.
      // This finds the equivalent of std::upper_bound index.
      while (b_idx < b_size && b_ptr[b_idx] <= p_val) {
        b_idx++;
      }
      
      current_sum += static_cast<uint64_t>(b_idx);
    }
    final_sum = current_sum;
  }

  return final_sum;
}