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
  
  uint64_t hash = 0;
  const size_t n_left = left_keys.size();
  const size_t n_right = right_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t i = 0;
    size_t j = 0;
    
    uint32_t last_key = 0;
    bool first = true;

    auto process_kv = [&](uint32_t k, uint32_t v) {
      if (!first && k == last_key) {
        // This case shouldn't happen with sorted unique inputs except between runs,
        // but the logic requires keeping the 'last' (right) value.
        // However, since we merge and right takes precedence, we handle it in the merge logic.
      }
      hash = mix(hash, k);
      hash = mix(hash, v);
      last_key = k;
      first = false;
    };

    while (i < n_left && j < n_right) {
      uint32_t lk = left_keys[i];
      uint32_t rk = right_keys[j];

      if (lk < rk) {
        hash = mix(hash, lk);
        hash = mix(hash, left_values[i]);
        i++;
      } else if (rk < lk) {
        hash = mix(hash, rk);
        hash = mix(hash, right_values[j]);
        j++;
      } else {
        // Keys equal: keep right value, skip both keys
        hash = mix(hash, rk);
        hash = mix(hash, right_values[j]);
        i++;
        j++;
      }
    }

    while (i < n_left) {
      hash = mix(hash, left_keys[i]);
      hash = mix(hash, left_values[i]);
      i++;
    }

    while (j < n_right) {
      hash = mix(hash, right_keys[j]);
      hash = mix(hash, right_values[j]);
      j++;
    }
  }

  return hash;
}