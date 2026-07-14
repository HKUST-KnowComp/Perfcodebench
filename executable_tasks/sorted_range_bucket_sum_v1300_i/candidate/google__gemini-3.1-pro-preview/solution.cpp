#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (probes.empty()) {
    return 0;
  }

  uint64_t total_sum = 0;
  const size_t b_size = boundaries.size();
  const size_t p_size = probes.size();
  const uint32_t* b_ptr = boundaries.data();
  const uint32_t* p_ptr = probes.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t b_idx = 0;

    // Since both boundaries and probes are sorted, we use a two-pointer approach.
    // This reduces complexity from O(P log B) to O(P + B) per iteration.
    for (size_t p_idx = 0; p_idx < p_size; ++p_idx) {
      uint32_t probe = p_ptr[p_idx];
      
      // Advance boundary pointer as long as the boundary is less than or equal to the probe.
      // The bucket index for 'probe' is the count of boundaries <= probe.
      while (b_idx < b_size && b_ptr[b_idx] <= probe) {
        b_idx++;
      }
      current_sum += static_cast<uint64_t>(b_idx);
    }
    total_sum = current_sum;
  }

  return total_sum;
}