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
    size_t left_idx = 0;
    size_t right_idx = 0;
    hash = 1469598103934665603ULL;
    while (left_idx < left_keys.size() || right_idx < right_keys.size()) {
      uint32_t key;
      uint32_t value;
      if (right_idx >= right_keys.size() ||
          (left_idx < left_keys.size() &&
           left_keys[left_idx] < right_keys[right_idx])) {
        key = left_keys[left_idx];
        value = left_values[left_idx];
        left_idx++;
      } else if (left_idx >= left_keys.size() ||
                 (right_idx < right_keys.size() &&
                  right_keys[right_idx] < left_keys[left_idx])) {
        key = right_keys[right_idx];
        value = right_values[right_idx];
        right_idx++;
      } else {
        key = left_keys[left_idx];
        value = right_values[right_idx];
        left_idx++;
        right_idx++;
      }
      hash = mix(hash, key);
      hash = mix(hash, value);
    }
  }
  return hash;
}
