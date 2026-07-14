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
      uint32_t l_key = left_keys[i];
      uint32_t r_key = right_keys[j];

      if (l_key < r_key) {
        hash = mix(hash, l_key);
        hash = mix(hash, left_values[i]);
        i++;
      } else if (l_key > r_key) {
        hash = mix(hash, r_key);
        hash = mix(hash, right_values[j]);
        j++;
      } else {
        // Keep last: skip left, take right
        hash = mix(hash, r_key);
        hash = mix(hash, right_values[j]);
        i++;
        j++;
      }
    }

    // Process remaining left
    while (i < n_left) {
      uint32_t key = left_keys[i];
      uint32_t val = left_values[i];
      // Handle potential duplicates within the left run itself (though usually sorted unique)
      // The baseline used stable_sort and then deduped, so we must handle internal duplicates
      if (i + 1 < n_left && left_keys[i+1] == key) {
        i++;
        continue;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
      i++;
    }

    // Process remaining right
    while (j < n_right) {
      uint32_t key = right_keys[j];
      uint32_t val = right_values[j];
      if (j + 1 < n_right && right_keys[j+1] == key) {
        j++;
        continue;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
      j++;
    }
    final_hash = hash;
  }
  return final_hash;
}
