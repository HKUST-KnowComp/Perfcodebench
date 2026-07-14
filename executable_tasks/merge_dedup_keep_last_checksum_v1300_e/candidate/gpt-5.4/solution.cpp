#include "interface.h"

#include <cstdint>
#include <vector>

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
  const std::size_t ln = left_keys.size();
  const std::size_t rn = right_keys.size();

  if (iters <= 0) return 0;

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    std::size_t j = 0;
    hash = 1469598103934665603ULL;

    while (i < ln || j < rn) {
      uint32_t key;
      uint32_t value;

      if (j >= rn || (i < ln && left_keys[i] < right_keys[j])) {
        key = left_keys[i];
        value = left_values[i];
        ++i;
        while (i < ln && left_keys[i] == key) {
          value = left_values[i];
          ++i;
        }
      } else if (i >= ln || right_keys[j] < left_keys[i]) {
        key = right_keys[j];
        value = right_values[j];
        ++j;
        while (j < rn && right_keys[j] == key) {
          value = right_values[j];
          ++j;
        }
      } else {
        key = right_keys[j];
        while (i < ln && left_keys[i] == key) {
          ++i;
        }
        value = right_values[j];
        ++j;
        while (j < rn && right_keys[j] == key) {
          value = right_values[j];
          ++j;
        }
      }

      hash = mix(hash, key);
      hash = mix(hash, value);
    }
  }

  return hash;
}
