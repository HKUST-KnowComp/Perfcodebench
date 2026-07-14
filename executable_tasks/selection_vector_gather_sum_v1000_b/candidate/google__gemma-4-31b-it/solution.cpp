#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const uint32_t* __restrict__ p_keys = keys.data();
  const uint8_t* __restrict__ p_flags = flags.data();
  const uint32_t* __restrict__ p_payloads = payloads.data();
  const size_t n = keys.size();
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Combine predicates to allow compiler to optimize branching/masking
      if (p_flags[i] && p_keys[i] < threshold && (p_keys[i] & 7u) == 0u) {
        current_sum += p_payloads[i];
      }
    }
    total_sum = current_sum;
  }
  return total_sum;
}