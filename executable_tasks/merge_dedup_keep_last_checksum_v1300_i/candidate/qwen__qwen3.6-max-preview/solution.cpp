#include "interface.h"
#include <cstdint>
#include <vector>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  
  const std::size_t n = left_keys.size();
  const std::size_t m = right_keys.size();
  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t i = 0, j = 0;
    uint32_t last_key = 0, last_val = 0;
    bool has_last = false;

    while (i < n && j < m) {
      uint32_t k, v;
      if (lk[i] <= rk[j]) {
        k = lk[i]; v = lv[i]; ++i;
      } else {
        k = rk[j]; v = rv[j]; ++j;
      }
      if (has_last && last_key == k) {
        last_val = v;
      } else {
        if (has_last) {
          hash = mix(hash, last_key);
          hash = mix(hash, last_val);
        }
        last_key = k;
        last_val = v;
        has_last = true;
      }
    }
    while (i < n) {
      uint32_t k = lk[i], v = lv[i]; ++i;
      if (has_last && last_key == k) {
        last_val = v;
      } else {
        if (has_last) {
          hash = mix(hash, last_key);
          hash = mix(hash, last_val);
        }
        last_key = k;
        last_val = v;
        has_last = true;
      }
    }
    while (j < m) {
      uint32_t k = rk[j], v = rv[j]; ++j;
      if (has_last && last_key == k) {
        last_val = v;
      } else {
        if (has_last) {
          hash = mix(hash, last_key);
          hash = mix(hash, last_val);
        }
        last_key = k;
        last_val = v;
        has_last = true;
      }
    }
    if (has_last) {
      hash = mix(hash, last_key);
      hash = mix(hash, last_val);
    }
  }
  return hash;
}