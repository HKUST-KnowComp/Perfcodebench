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

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    std::size_t i = 0;
    std::size_t j = 0;

    while (i < ln && j < rn) {
      const uint32_t lk = left_keys[i];
      const uint32_t rk = right_keys[j];

      if (lk < rk) {
        uint32_t key = lk;
        uint32_t value = left_values[i];
        ++i;
        while (i < ln && left_keys[i] == key) {
          value = left_values[i];
          ++i;
        }
        h = mix(h, key);
        h = mix(h, value);
      } else if (rk < lk) {
        uint32_t key = rk;
        uint32_t value = right_values[j];
        ++j;
        while (j < rn && right_keys[j] == key) {
          value = right_values[j];
          ++j;
        }
        h = mix(h, key);
        h = mix(h, value);
      } else {
        const uint32_t key = lk;
        while (i < ln && left_keys[i] == key) {
          ++i;
        }
        uint32_t value = right_values[j];
        ++j;
        while (j < rn && right_keys[j] == key) {
          value = right_values[j];
          ++j;
        }
        h = mix(h, key);
        h = mix(h, value);
      }
    }

    while (i < ln) {
      uint32_t key = left_keys[i];
      uint32_t value = left_values[i];
      ++i;
      while (i < ln && left_keys[i] == key) {
        value = left_values[i];
        ++i;
      }
      h = mix(h, key);
      h = mix(h, value);
    }

    while (j < rn) {
      uint32_t key = right_keys[j];
      uint32_t value = right_values[j];
      ++j;
      while (j < rn && right_keys[j] == key) {
        value = right_values[j];
        ++j;
      }
      h = mix(h, key);
      h = mix(h, value);
    }

    hash = h;
  }

  return hash;
}
