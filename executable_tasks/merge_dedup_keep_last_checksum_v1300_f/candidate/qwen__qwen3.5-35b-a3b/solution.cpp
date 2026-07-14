#pragma once

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  uint64_t hash = 0;
  const uint64_t MIX_MULT = 1099511628211ULL;
  const uint64_t INIT_HASH = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = INIT_HASH;
    size_t i = 0, j = 0;
    uint32_t last_key = 0;
    bool has_last = false;

    while (i < left_keys.size() || j < right_keys.size()) {
      uint32_t key;
      uint32_t value;

      if (i < left_keys.size() && (j >= right_keys.size() || left_keys[i] <= right_keys[j])) {
        key = left_keys[i];
        value = left_values[i];
        ++i;
      } else {
        key = right_keys[j];
        value = right_values[j];
        ++j;
      }

      if (has_last && key == last_key) {
        // Keep last value (right-side wins)
        hash ^= value;
        hash *= MIX_MULT;
      } else {
        // New key
        if (has_last) {
          hash ^= last_key;
          hash *= MIX_MULT;
          hash ^= last_value;
          hash *= MIX_MULT;
        }
        last_key = key;
        last_value = value;
        has_last = true;
      }
    }

    // Finalize last key-value pair
    if (has_last) {
      hash ^= last_key;
      hash *= MIX_MULT;
      hash ^= last_value;
      hash *= MIX_MULT;
    }
  }

  return hash;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}