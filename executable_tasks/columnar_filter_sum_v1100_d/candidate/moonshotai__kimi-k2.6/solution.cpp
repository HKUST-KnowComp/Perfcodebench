#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = keys.size();
  const uint32_t* keys_data = keys.data();
  const uint32_t* values_data = values.data();
  const uint8_t* flags_data = flags.data();

  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (flags_data[i] && keys_data[i] < key_threshold && ((keys_data[i] & 3u) == 0u)) {
      sum += static_cast<uint64_t>(values_data[i]);
    }
  }
  return sum;
}
