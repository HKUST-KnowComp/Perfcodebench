#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t total_sum = 0;
  const uint32_t* b_ptr = boundaries.data();
  const uint32_t* b_end = b_ptr + boundaries.size();
  const uint32_t* p_ptr = probes.data();
  const uint32_t* p_end = p_ptr + probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    size_t b_idx = 0;
    const size_t b_size = boundaries.size();

    for (size_t p_idx = 0; p_idx < probes.size(); ++p_idx) {
      uint32_t probe = probes[p_idx];
      // Since probes are sorted, we only need to advance the boundary pointer
      while (b_idx < b_size && b_ptr[b_idx] <= probe) {
        b_idx++;
      }
      iter_sum += b_idx;
    }
    total_sum = iter_sum;
  }
  return total_sum;
}