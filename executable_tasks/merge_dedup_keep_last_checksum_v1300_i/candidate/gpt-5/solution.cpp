#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  const std::size_t ln = left_keys.size();
  const std::size_t rn = right_keys.size();

  // Early exit optimization for completely empty inputs across iterations
  if (ln == 0 && rn == 0) {
    // If iters == 0, baseline returns 0; if iters > 0, FNV basis with no data hashed
    if (iters > 0) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    std::size_t j = 0;
    hash = 1469598103934665603ULL;

    while (i < ln || j < rn) {
      uint32_t key;
      uint32_t value;

      if (i < ln && (j >= rn || left_keys[i] < right_keys[j])) {
        // Next key comes from left only
        key = left_keys[i];
        uint32_t v = left_values[i];
        ++i;
        while (i < ln && left_keys[i] == key) {
          v = left_values[i];
          ++i;
        }
        value = v;
      } else if (j < rn && (i >= ln || right_keys[j] < left_keys[i])) {
        // Next key comes from right only
        key = right_keys[j];
        uint32_t v = right_values[j];
        ++j;
        while (j < rn && right_keys[j] == key) {
          v = right_values[j];
          ++j;
        }
        value = v;
      } else {
        // Keys equal on both sides
        key = left_keys[i]; // same as right_keys[j]
        // Advance left past all duplicates of this key
        do {
          ++i;
        } while (i < ln && left_keys[i] == key);
        // Take the last value from the right group for this key
        uint32_t v = right_values[j];
        ++j;
        while (j < rn && right_keys[j] == key) {
          v = right_values[j];
          ++j;
        }
        value = v;
      }

      hash = mix(hash, static_cast<uint64_t>(key));
      hash = mix(hash, static_cast<uint64_t>(value));
    }
  }

  return hash;
}
