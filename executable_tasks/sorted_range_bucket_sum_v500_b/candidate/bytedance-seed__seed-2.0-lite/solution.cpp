#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t n_bound = boundaries.size();
  const uint32_t* const bound_ptr = boundaries.data();
  const size_t n_probe = probes.size();
  const uint32_t* const probe_ptr = probes.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t ptr = 0;
    for (size_t i = 0; i < n_probe; ++i) {
      const uint32_t probe = probe_ptr[i];
      while (ptr < n_bound && bound_ptr[ptr] <= probe) {
        ++ptr;
      }
      sum += ptr;
    }
  }
  return sum;
}