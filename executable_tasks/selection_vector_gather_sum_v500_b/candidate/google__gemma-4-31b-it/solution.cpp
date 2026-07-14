#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const size_t n = keys.size();
  const uint32_t* p_keys = keys.data();
  const uint8_t* p_flags = flags.data();
  const uint32_t* p_payloads = payloads.data();
  
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Combine predicates into a single check to avoid branching where possible
      // flags[i] is 1 or 0, keys[i] < threshold, and keys[i] % 8 == 0
      if (p_flags[i] && p_keys[i] < threshold && (p_keys[i] & 7u) == 0u) {
        current_sum += p_payloads[i];
      }
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}