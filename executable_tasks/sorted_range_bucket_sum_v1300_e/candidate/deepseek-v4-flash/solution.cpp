#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t final_sum = 0;
  const size_t m = boundaries.size();
  const size_t n = probes.size();
  const uint32_t* b_ptr = boundaries.data();
  const uint32_t* p_ptr = probes.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t b = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t probe = p_ptr[i];
      while (b < m && b_ptr[b] <= probe) {
        ++b;
      }
      sum += b;
    }
    final_sum = sum;
  }
  return final_sum;
}
