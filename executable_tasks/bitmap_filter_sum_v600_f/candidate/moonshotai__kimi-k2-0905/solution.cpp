#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  const uint32_t words = (key_space + 63) >> 6;
  std::vector<uint64_t> bitmap(words, 0);

  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_vals = values.data();
  const size_t n = probe_keys.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // reset bitmap
    std::memset(bitmap.data(), 0, words * sizeof(uint64_t));

    // set enabled bits
    for (uint32_t k : enabled_keys) {
      bitmap[k >> 6] |= uint64_t(1) << (k & 63);
    }

    // sum qualifying rows
    uint64_t local_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      const uint32_t k = p_keys[i];
      const uint64_t mask = -(bitmap[k >> 6] >> (k & 63) & 1);
      local_sum += (p_vals[i] & mask);
    }
    sum = local_sum;
  }
  return sum;
}