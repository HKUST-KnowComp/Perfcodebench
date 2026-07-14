#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  const auto n = keys.size();
  const auto* keys_ptr = keys.data();
  const auto* values_ptr = values.data();
  const auto* flags_ptr = flags.data();
  for (std::size_t i = 0; i < n; ++i) {
    if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3) == 0)) {
      sum += static_cast<uint64_t>(values_ptr[i]);
    }
  }
  return sum;
}