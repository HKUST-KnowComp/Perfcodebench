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
  const std::size_t ln = left_keys.size();
  const std::size_t rn = right_keys.size();
  const uint32_t* lk = ln ? left_keys.data() : nullptr;
  const uint32_t* lv = ln ? left_values.data() : nullptr;
  const uint32_t* rk = rn ? right_keys.data() : nullptr;
  const uint32_t* rv = rn ? right_values.data() : nullptr;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t li = 0;
    std::size_t ri = 0;

    while (li < ln && ri < rn) {
      uint32_t lkey = lk[li];
      uint32_t rkey = rk[ri];
      if (lkey < rkey) {
        hash = mix(hash, lkey);
        hash = mix(hash, lv[li]);
        ++li;
      } else if (rkey < lkey) {
        hash = mix(hash, rkey);
        hash = mix(hash, rv[ri]);
        ++ri;
      } else {
        // Duplicate key: keep-last means right value wins
        hash = mix(hash, rkey);
        hash = mix(hash, rv[ri]);
        ++li;
        ++ri;
      }
    }
    while (li < ln) {
      hash = mix(hash, lk[li]);
      hash = mix(hash, lv[li]);
      ++li;
    }
    while (ri < rn) {
      hash = mix(hash, rk[ri]);
      hash = mix(hash, rv[ri]);
      ++ri;
    }
  }
  return hash;
}
