#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  if (iters <= 0 || n == 0) {
    return 0;
  }

  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t key = k[i];
      if (f[i] && key < key_threshold && ((key & 3u) == 0u)) {
        local_sum += static_cast<uint64_t>(v[i]);
      }
    }
    sum = local_sum;
  }
  return sum;
}
