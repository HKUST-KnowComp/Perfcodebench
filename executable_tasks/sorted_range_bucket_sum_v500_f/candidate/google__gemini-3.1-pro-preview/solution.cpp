#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (probes.empty()) return 0;

  const uint32_t* b_ptr = boundaries.data();
  const size_t b_size = boundaries.size();
  const uint32_t* p_ptr = probes.data();
  const size_t p_size = probes.size();

  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t b_idx = 0;

    for (size_t p_idx = 0; p_idx < p_size; ) {
      uint32_t p_val = p_ptr[p_idx];
      
      // Count duplicate probes to process them in one batch
      size_t next_p_idx = p_idx + 1;
      while (next_p_idx < p_size && p_ptr[next_p_idx] == p_val) {
        next_p_idx++;
      }
      size_t p_count = next_p_idx - p_idx;

      // Galloping search for the next boundary index
      // This is efficient when probes are sparse relative to boundaries
      if (b_idx < b_size && b_ptr[b_idx] <= p_val) {
        size_t step = 1;
        while (b_idx + step < b_size && b_ptr[b_idx + step] <= p_val) {
          b_idx += step;
          step <<= 1;
        }
        // Refine the search in the remaining range [b_idx, min(b_idx + step, b_size)]
        size_t search_end = (b_idx + step < b_size) ? b_idx + step : b_size;
        b_idx = std::upper_bound(b_ptr + b_idx, b_ptr + search_end, p_val) - b_ptr;
      }

      current_sum += (uint64_t)p_count * b_idx;
      p_idx = next_p_idx;
    }
    last_sum = current_sum;
  }

  return last_sum;
}