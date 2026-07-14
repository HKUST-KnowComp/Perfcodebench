#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t sum = 0;
  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t key = k[i];
    if (f[i] && key < key_threshold && ((key & 3u) == 0u)) {
      sum += v[i];
    }
  }

  return sum;
}
