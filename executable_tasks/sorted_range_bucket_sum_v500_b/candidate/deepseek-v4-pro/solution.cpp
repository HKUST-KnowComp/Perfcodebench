#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  const uint32_t* b = boundaries.data();
  const size_t n_b = boundaries.size();
  const uint32_t* p = probes.data();
  const uint32_t* p_end = p + probes.size();

  uint64_t sum = 0;
  size_t bucket_idx = 0;
  for (; p != p_end; ++p) {
    while (bucket_idx < n_b && b[bucket_idx] <= *p) {
      ++bucket_idx;
    }
    sum += bucket_idx;
  }
  return sum * static_cast<uint64_t>(iters);
}
