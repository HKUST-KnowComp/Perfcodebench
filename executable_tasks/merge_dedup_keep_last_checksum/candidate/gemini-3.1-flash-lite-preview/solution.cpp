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
  const size_t n = left_keys.size();
  const size_t m = right_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t i = 0, j = 0;

    while (i < n || j < m) {
      uint32_t key;
      uint32_t val;

      if (i < n && (j == m || left_keys[i] < right_keys[j])) {
        key = left_keys[i];
        val = left_values[i];
        i++;
      } else if (j < m && (i == n || right_keys[j] < left_keys[i])) {
        key = right_keys[j];
        val = right_values[j];
        j++;
      } else {
        // Keys are equal, keep right-side value
        key = left_keys[i];
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