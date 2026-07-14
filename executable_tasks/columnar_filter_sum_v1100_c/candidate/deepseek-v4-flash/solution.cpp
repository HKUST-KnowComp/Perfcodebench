#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  if (iters <= 0) return 0;
  const size_t n = keys.size();
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  const uint8_t* flags_ptr = flags.data();
  uint64_t sum = 0;
  for (size_t i = 0; i < n; ++i) {
    if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
      sum += values_ptr[i];
    }
  }
  return sum;
}