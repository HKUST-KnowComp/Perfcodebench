#include "interface.h"

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
      if (left_keys[i] < right_keys[j]) {
        hash = mix(hash, left_keys[i]);
        hash = mix(hash, left_values[i]);
        i++;
      } else if (right_keys[j] < left_keys[i]) {
        hash = mix(hash, right_keys[j]);
        hash = mix(hash, right_values[j]);
        j++;
      } else {
        // Keep right value on duplicate, advance both pointers
        hash = mix(hash, right_keys[j]);
        hash = mix(hash, right_values[j]);
        i++;
        j++;
      }
    }

    // Add remaining left elements
    while (i < left_len) {
      hash = mix(hash, left_keys[i]);
      hash = mix(hash, left_values[i]);
      i++;
    }
    // Add remaining right elements
    while (j < right_len) {
      hash = mix(hash, right_keys[j]);
      hash = mix(hash, right_values[j]);
      j++;
    }

    final_hash = hash;
  }
  return final_hash;
}