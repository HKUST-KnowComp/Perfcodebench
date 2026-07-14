#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  uint64_t final_hash = 0;
  const size_t L = left_keys.size();
  const size_t R = right_keys.size();
  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t i = 0, j = 0;
    uint32_t cur_key = 0, cur_val = 0;
    bool has_cur = false;

    while (i < L && j < R) {
      uint32_t k1 = lk[i];
      uint32_t k2 = rk[j];
      uint32_t k, v;
      if (k1 < k2) {
        k = k1; v = lv[i]; ++i;
      } else if (k1 > k2) {
        k = k2; v = rv[j]; ++j;
      } else {
        k = k1;
        while (i < L && lk[i] == k) ++i;
        v = rv[j]; ++j;
      }

      if (has_cur && k == cur_key) {
        cur_val = v;
      } else {
        if (has_cur) {
          hash ^= cur_key;
          hash *= 1099511628211ULL;
          hash ^= cur_val;
          hash *= 1099511628211ULL;
        }
        cur_key = k;
        cur_val = v;
        has_cur = true;
      }
    }
    while (i < L) {
      uint32_t k = lk[i];
      uint32_t v = lv[i];
      ++i;
      if (has_cur && k == cur_key) {
        cur_val = v;
      } else {
        if (has_cur) {
          hash ^= cur_key;
          hash *= 1099511628211ULL;
          hash ^= cur_val;
          hash *= 1099511628211ULL;
        }
        cur_key = k;
        cur_val = v;
        has_cur = true;
      }
    }
    while (j < R) {
      uint32_t k = rk[j];
      uint32_t v = rv[j];
      ++j;
      if (has_cur && k == cur_key) {
        cur_val = v;
      } else {
        if (has_cur) {
          hash ^= cur_key;
          hash *= 1099511628211ULL;
          hash ^= cur_val;
          hash *= 1099511628211ULL;
        }
        cur_key = k;
        cur_val = v;
        has_cur = true;
      }
    }
    if (has_cur) {
      hash ^= cur_key;
      hash *= 1099511628211ULL;
      hash ^= cur_val;
      hash *= 1099511628211ULL;
    }
    final_hash = hash;
  }
  return final_hash;
}