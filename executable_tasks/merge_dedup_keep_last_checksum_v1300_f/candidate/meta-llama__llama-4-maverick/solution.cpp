#include "interface.h"

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
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t left_idx = left_keys.size();
    size_t right_idx = right_keys.size();
    hash = 1469598103934665603ULL;
    while (left_idx > 0 || right_idx > 0) {
      uint32_t key;
      uint32_t value;
      if (right_idx == 0 || (left_idx > 0 && left_keys[left_idx - 1] > right_keys[right_idx - 1])) {
        --left_idx;
        key = left_keys[left_idx];
        value = left_values[left_idx];
      } else {
        --right_idx;
        key = right_keys[right_idx];
        value = right_values[right_idx];
      }
      if (left_idx > 0 && left_keys[left_idx - 1] == key) {
        continue;
      }
      if (right_idx > 0 && right_keys[right_idx] == key) {
        value = right_values[right_idx];
      }
      hash = mix(hash, key);
      hash = mix(hash, value);
    }
  }
  return hash;
}
