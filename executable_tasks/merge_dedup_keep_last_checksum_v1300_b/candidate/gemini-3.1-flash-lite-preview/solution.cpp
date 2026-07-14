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
  size_t n1 = left_keys.size();
  size_t n2 = right_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t i = 0, j = 0;

    while (i < n1 || j < n2) {
      uint32_t key, val;
      if (i < n1 && (j == n2 || left_keys[i] < right_keys[j])) {
        key = left_keys[i];
        val = left_values[i];
        i++;
      } else if (j < n2 && (i == n1 || right_keys[j] < left_keys[i])) {
        key = right_keys[j];
        val = right_values[j];
        j++;
      } else {
        // Keys are equal, keep right-side value
        key = right_keys[j];
        val = right_values[j];
        i++;
        j++;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
    }
    final_hash = hash;
  }
  return final_hash;
}