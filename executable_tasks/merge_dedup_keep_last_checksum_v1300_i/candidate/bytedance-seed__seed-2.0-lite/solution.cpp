#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  uint64_t final_hash = 0;
  const size_t left_len = left_keys.size();
  const size_t right_len = right_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint64_t hash = 1469598103934665603ULL;
    while (i < left_len && j < right_len) {
      const uint32_t lk = left_keys[i];
      const uint32_t rk = right_keys[j];
      if (lk < rk) {
        hash = mix(hash, lk);
        hash = mix(hash, left_values[i]);
        ++i;
      } else if (rk < lk) {
        hash = mix(hash, rk);
        hash = mix(hash, right_values[j]);
        ++j;
      } else {
        hash = mix(hash, lk);
        hash = mix(hash, right_values[j]);
        ++i;
        ++j;
      }
    }
    while (i < left_len) {
      hash = mix(hash, left_keys[i]);
      hash = mix(hash, left_values[i]);
      ++i;
    }
    while (j < right_len) {
      hash = mix(hash, right_keys[j]);
      hash = mix(hash, right_values[j]);
      ++j;
    }
    final_hash = hash;
  }
  return final_hash;
}