#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = keys.size();
  const uint32_t* k_ptr = keys.data();
  const uint8_t* f_ptr = flags.data();
  const uint32_t* p_ptr = payloads.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Combine predicate checks to minimize branching and avoid intermediate storage
      if (f_ptr[i] && k_ptr[i] < threshold && ((k_ptr[i] & 7u) == 0u)) {
        iter_sum += p_ptr[i];
      }
    }
    total_sum = iter_sum;
  }
  return total_sum;
}