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
      if (i < n_left && (j == n_right || left_keys[i] < right_keys[j])) {
        key = left_keys[i];
        val = left_values[i];
        i++;
      } else if (j < n_right && (i == n_left || right_keys[j] < left_keys[i])) {
        key = right_keys[j];
        val = right_values[j];
        j++;
      } else {
        // Keys are equal: keep the right-side value
        key = left_keys[i];
        val = right_values[j];
        i++;
        j++;
      }

      // Handle potential duplicates within the same run (though problem implies sorted runs)
      // The baseline logic: if current key == previous key, update value. 
      // Since we process in sorted order, we only need to check if the next elements 
      // have the same key to effectively "keep last".
      
      // To correctly implement "keep last" for sequences of identical keys:
      while (i < n_left && left_keys[i] == key) {
        val = left_values[i++];
      }
      while (j < n_right && right_keys[j] == key) {
        val = right_values[j++];
      }

      hash = mix(hash, (uint64_t)key);
      hash = mix(hash, (uint64_t)val);
    }
    final_hash = hash;
  }
  return final_hash;
}