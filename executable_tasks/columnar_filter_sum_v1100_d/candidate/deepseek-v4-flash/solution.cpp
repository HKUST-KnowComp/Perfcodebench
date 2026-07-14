#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  std::size_t n = keys.size();
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* vals_ptr = values.data();
  const uint8_t* flags_ptr = flags.data();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
        sum += static_cast<uint64_t>(vals_ptr[i]);
      }
    }
  }
  return sum;
}
