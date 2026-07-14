#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // one-time build of a dense bitset
  const uint32_t n64 = (key_space + 63) / 64;
  std::vector<uint64_t> bitmap(n64, 0);
  for (uint32_t k : enabled_keys) {
    bitmap[k >> 6] |= 1ULL << (k & 63);
  }

  uint64_t sum = 0;
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pv = values.data();
  const size_t n = probe_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    for (size_t i = 0; i < n; ++i) {
      const uint32_t key = pk[i];
      const uint64_t bit = 1ULL << (key & 63);
      if ((bitmap[key >> 6] & bit) != 0) {
        local += static_cast<uint64_t>(pv[i]);
      }
    }
    sum = local;
  }
  return sum;
}