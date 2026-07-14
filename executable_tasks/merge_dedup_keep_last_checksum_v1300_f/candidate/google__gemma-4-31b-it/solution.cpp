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

    while (i < n_left || j < n_right) {
      uint32_t key, val;
      if (i < n_left && j < n_right) {
        if (left_keys[i] < right_keys[j]) {
          key = left_keys[i];
          val = left_values[i];
          ++i;
        } else if (left_keys[i] > right_keys[j]) {
          key = right_keys[j];
          val = right_values[j];
          ++j;
        } else {
          // Keys are equal: keep the right-side value
          key = right_keys[j];
          val = right_values[j];
          ++i;
          ++j;
        }
      } else if (i < n_left) {
        key = left_keys[i];
        val = left_values[i];
        ++i;
      } else {
        key = right_keys[j];
        val = right_values[j];
        ++j;
      }

      // Handle duplicates within the same run (if they exist) or across runs
      // The problem states runs are sorted. If runs can have internal duplicates,
      // we must ensure only the last one is hashed. 
      // However, the baseline logic 'out.back().first == kv.first' implies 
      // we only care about the final value for a key.
      
      // To correctly emulate the baseline's 'out' vector behavior with the merge:
      // We need to peek ahead to see if the next element has the same key.
      while ((i < n_left && left_keys[i] == key) || (j < n_right && right_keys[j] == key)) {
        if (i < n_left && left_keys[i] == key) {
          // If it's from left, it's only relevant if no right-side value exists for this key
          // But since we are merging, we just advance i.
          ++i;
        }
        if (j < n_right && right_keys[j] == key) {
          val = right_values[j]; // Update to the latest value for this key
          ++j;
        }
      }
      
      hash = mix(hash, (uint64_t)key);
      hash = mix(hash, (uint64_t)val);
    }
    final_hash = hash;
  }
  return final_hash;
}
