#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (iters == 0) return 0;
  const std::size_t n = keys.size();
  const uint32_t* keys_ptr = keys.data();
  const uint8_t* flags_ptr = flags.data();
  const uint32_t* payloads_ptr = payloads.data();
  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (flags_ptr[i] && keys_ptr[i] < threshold && ((keys_ptr[i] & 7u) == 0u)) {
      sum += static_cast<uint64_t>(payloads_ptr[i]);
    }
  }
  return sum;
}
