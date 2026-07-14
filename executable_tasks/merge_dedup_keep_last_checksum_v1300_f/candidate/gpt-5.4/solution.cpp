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

  const uint32_t* lk = ln ? left_keys.data() : nullptr;
  const uint32_t* lv = ln ? left_values.data() : nullptr;
  const uint32_t* rk = rn ? right_keys.data() : nullptr;
  const uint32_t* rv = rn ? right_values.data() : nullptr;

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    std::size_t j = 0;
    hash = 1469598103934665603ULL;

    while (i < ln && j < rn) {
      const uint32_t lkey = lk[i];
      const uint32_t rkey = rk[j];

      if (lkey < rkey) {
        hash = mix(hash, lkey);
        hash = mix(hash, lv[i]);
        ++i;
      } else if (rkey < lkey) {
        hash = mix(hash, rkey);
        hash = mix(hash, rv[j]);
        ++j;
      } else {
        const uint32_t key = lkey;

        while (i + 1 < ln && lk[i + 1] == key) {
          ++i;
        }
        while (j + 1 < rn && rk[j + 1] == key) {
          ++j;
        }

        hash = mix(hash, key);
        hash = mix(hash, rv[j]);
        ++i;
        ++j;
      }
    }

    while (i < ln) {
      const uint32_t key = lk[i];
      while (i + 1 < ln && lk[i + 1] == key) {
        ++i;
      }
      hash = mix(hash, key);
      hash = mix(hash, lv[i]);
      ++i;
    }

    while (j < rn) {
      const uint32_t key = rk[j];
      while (j + 1 < rn && rk[j + 1] == key) {
        ++j;
      }
      hash = mix(hash, key);
      hash = mix(hash, rv[j]);
      ++j;
    }
  }

  return hash;
}
