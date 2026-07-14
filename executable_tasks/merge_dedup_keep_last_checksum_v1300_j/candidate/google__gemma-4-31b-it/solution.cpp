#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  const size_t n_left = left_keys.size();
  const size_t n_right = right_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t i = 0, j = 0;

    while (i < n_left && j < n_right) {
      if (left_keys[i] < right_keys[j]) {
        hash = mix(hash, left_keys[i]);
        hash = mix(hash, left_values[i]);
        ++i;
      } else if (left_keys[i] > right_keys[j]) {
        hash = mix(hash, right_keys[j]);
        hash = mix(hash, right_values[j]);
        ++j;
      } else {
        // Keys are equal: keep-last means we take the right value
        hash = mix(hash, right_keys[j]);
        hash = mix(hash, right_values[j]);
        ++i;
        ++j;
      }
    }

    while (i < n_left) {
      hash = mix(hash, left_keys[i]);
      hash = mix(hash, left_values[i]);
      ++i;
    }
    while (j < n_right) {
      hash = mix(hash, right_keys[j]);
      hash = mix(hash, right_values[j]);
      ++j;
    }
    final_hash = hash;
  }
  return final_hash;
}