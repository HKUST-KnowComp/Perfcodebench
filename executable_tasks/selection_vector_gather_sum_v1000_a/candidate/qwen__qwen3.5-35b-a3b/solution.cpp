#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t sum = 0;
  const auto* keys_data = keys.data();
  const auto* flags_data = flags.data();
  const auto* payloads_data = payloads.data();
  std::size_t n = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (flags_data[i] && keys_data[i] < threshold && (keys_data[i] & 7u) == 0u) {
        current_sum += payloads_data[i];
      }
    }
    sum = current_sum;
  }
  return sum;
}