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
  const size_t n = left_keys.size();
  const size_t m = right_keys.size();

  const uint32_t* lk_ptr = left_keys.data();
  const uint32_t* lv_ptr = left_values.data();
  const uint32_t* rk_ptr = right_keys.data();
  const uint32_t* rv_ptr = right_values.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t i = 0, j = 0;

    while (i < n && j < m) {
      uint32_t lk = lk_ptr[i];
      uint32_t rk = rk_ptr[j];

      if (lk < rk) {
        uint32_t val = lv_ptr[i];
        while (i + 1 < n && lk_ptr[i + 1] == lk) val = lv_ptr[++i];
        hash = mix(hash, lk);
        hash = mix(hash, val);
        i++;
      } else if (lk > rk) {
        uint32_t val = rv_ptr[j];
        while (j + 1 < m && rk_ptr[j + 1] == rk) val = rv_ptr[++j];
        hash = mix(hash, rk);
        hash = mix(hash, val);
        j++;
      } else {
        uint32_t val = rv_ptr[j];
        while (i + 1 < n && lk_ptr[i + 1] == lk) i++;
        while (j + 1 < m && rk_ptr[j + 1] == rk) val = rv_ptr[++j];
        hash = mix(hash, rk);
        hash = mix(hash, val);
        i++;
        j++;
      }
    }

    while (i < n) {
      uint32_t lk = lk_ptr[i];
      uint32_t val = lv_ptr[i];
      while (i + 1 < n && lk_ptr[i + 1] == lk) val = lv_ptr[++i];
      hash = mix(hash, lk);
      hash = mix(hash, val);
      i++;
    }

    while (j < m) {
      uint32_t rk = rk_ptr[j];
      uint32_t val = rv_ptr[j];
      while (j + 1 < m && rk_ptr[j + 1] == rk) val = rv_ptr[++j];
      hash = mix(hash, rk);
      hash = mix(hash, val);
      j++;
    }
  }
  return hash;
}
