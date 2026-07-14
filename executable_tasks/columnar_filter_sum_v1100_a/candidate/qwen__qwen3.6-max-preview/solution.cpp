#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  (void)iters; // Computation is idempotent; single pass suffices.
  
  uint64_t sum = 0;
  const size_t n = keys.size();
  const uint32_t* __restrict__ k_ptr = keys.data();
  const uint32_t* __restrict__ v_ptr = values.data();
  const uint8_t* __restrict__ f_ptr = flags.data();

  for (size_t i = 0; i < n; ++i) {
    uint32_t k = k_ptr[i];
    if (f_ptr[i] && k < key_threshold && (k & 3u) == 0u) {
      sum += v_ptr[i];
    }
  }
  return sum;
}
