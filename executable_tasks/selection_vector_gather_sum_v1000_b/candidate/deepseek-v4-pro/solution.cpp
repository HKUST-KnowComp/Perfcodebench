#include "interface.h"
#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int /*iters*/) {
  const std::size_t n = keys.size();
  const uint32_t* const key_ptr = keys.data();
  const uint8_t* const flag_ptr = flags.data();
  const uint32_t* const pay_ptr = payloads.data();
  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (flag_ptr[i] && key_ptr[i] < threshold && ((key_ptr[i] & 7u) == 0u)) {
      sum += static_cast<uint64_t>(pay_ptr[i]);
    }
  }
  return sum;
}