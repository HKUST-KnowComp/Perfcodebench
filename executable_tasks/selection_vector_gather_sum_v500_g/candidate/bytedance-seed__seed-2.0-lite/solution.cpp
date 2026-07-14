#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (keys.empty() || iters <= 0) return 0;
  
  const std::size_t n = keys.size();
  const uint8_t* __restrict__ flag_ptr = flags.data();
  const uint32_t* __restrict__ key_ptr = keys.data();
  const uint32_t* __restrict__ payload_ptr = payloads.data();
  
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (flag_ptr[i] && key_ptr[i] < threshold && ((key_ptr[i] & 7u) == 0u)) {
        sum += static_cast<uint64_t>(payload_ptr[i]);
      }
    }
    final_sum = sum;
  }
  return final_sum;
}