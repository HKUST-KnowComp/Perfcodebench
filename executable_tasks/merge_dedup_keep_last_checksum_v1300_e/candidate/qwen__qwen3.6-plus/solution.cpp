#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();
  const size_t ln = left_keys.size();
  const size_t rn = right_keys.size();
  const uint32_t* lk_end = lk + ln;
  const uint32_t* rk_end = rk + rn;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    const uint32_t* i_lk = lk;
    const uint32_t* i_lv = lv;
    const uint32_t* i_rk = rk;
    const uint32_t* i_rv = rv;

    uint32_t last_key = 0;
    uint32_t last_val = 0;
    bool has_last = false;

    while (i_lk < lk_end && i_rk < rk_end) {
      uint32_t k, v;
      if (*i_lk <= *i_rk) {
        k = *i_lk; v = *i_lv; ++i_lk; ++i_lv;
      } else {
        k = *i_rk; v = *i_rv; ++i_rk; ++i_rv;
      }
      if (has_last && k == last_key) {
        last_val = v;
      } else {
        if (has_last) {
          hash = (hash ^ last_key) * FNV_PRIME;
          hash = (hash ^ last_val) * FNV_PRIME;
        }
        last_key = k;
        last_val = v;
        has_last = true;
      }
    }
    while (i_lk < lk_end) {
      uint32_t k = *i_lk; uint32_t v = *i_lv; ++i_lk; ++i_lv;
      if (has_last && k == last_key) {
        last_val = v;
      } else {
        if (has_last) {
          hash = (hash ^ last_key) * FNV_PRIME;
          hash = (hash ^ last_val) * FNV_PRIME;
        }
        last_key = k;
        last_val = v;
        has_last = true;
      }
    }
    while (i_rk < rk_end) {
      uint32_t k = *i_rk; uint32_t v = *i_rv; ++i_rk; ++i_rv;
      if (has_last && k == last_key) {
        last_val = v;
      } else {
        if (has_last) {
          hash = (hash ^ last_key) * FNV_PRIME;
          hash = (hash ^ last_val) * FNV_PRIME;
        }
        last_key = k;
        last_val = v;
        has_last = true;
      }
    }
    if (has_last) {
      hash = (hash ^ last_key) * FNV_PRIME;
      hash = (hash ^ last_val) * FNV_PRIME;
    }
  }
  return hash;
}
