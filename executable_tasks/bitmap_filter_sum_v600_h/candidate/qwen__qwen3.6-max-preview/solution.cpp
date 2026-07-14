#include "interface.h"

#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  std::vector<uint8_t> bitmap(key_space, 0);
  uint64_t sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* val = values.data();
  uint8_t* bm = bitmap.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bm, 0, key_space);
    for (uint32_t k : enabled_keys) {
      bm[k] = 1;
    }

    uint64_t iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      if (bm[pk[i]]) {
        iter_sum += val[i];
      }
    }
    sum = iter_sum;
  }
  return sum;
}