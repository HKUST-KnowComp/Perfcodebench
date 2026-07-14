#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t total_sum = 0;
  const uint32_t* b_ptr = boundaries.data();
  const size_t b_size = boundaries.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (uint32_t probe : probes) {
      size_t low = 0;
      size_t high = b_size;
      while (low < high) {
        size_t mid = low + ((high - low) >> 1);
        if (probe < b_ptr[mid]) {
          high = mid;
        } else {
          low = mid + 1;
        }
      }
      current_iter_sum += low;
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}