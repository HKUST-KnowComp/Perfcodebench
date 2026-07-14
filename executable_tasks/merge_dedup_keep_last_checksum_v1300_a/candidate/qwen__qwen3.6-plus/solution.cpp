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
  
  const std::size_t L = left_keys.size();
  const std::size_t R = right_keys.size();
  uint64_t hash = 0;

  const uint32_t* lk_ptr = left_keys.data();
  const uint32_t* lv_ptr = left_values.data();
  const uint32_t* rk_ptr = right_keys.data();
  const uint32_t* rv_ptr = right_values.data();
  const uint32_t* lk_end = lk_ptr + L;
  const uint32_t* rk_end = rk_ptr + R;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint32_t* i = lk_ptr;
    const uint32_t* iv = lv_ptr;
    const uint32_t* j = rk_ptr;
    const uint32_t* jv = rv_ptr;
    
    while (i < lk_end && j < rk_end) {
      uint32_t lk = *i;
      uint32_t rk = *j;
      if (lk < rk) {
        hash = mix(hash, lk);
        hash = mix(hash, *iv);
        ++i; ++iv;
      } else if (lk > rk) {
        hash = mix(hash, rk);
        hash = mix(hash, *jv);
        ++j; ++jv;
      } else {
        hash = mix(hash, lk);
        hash = mix(hash, *jv);
        ++i; ++iv;
        ++j; ++jv;
      }
    }
    while (i < lk_end) {
      hash = mix(hash, *i);
      hash = mix(hash, *iv);
      ++i; ++iv;
    }
    while (j < rk_end) {
      hash = mix(hash, *j);
      hash = mix(hash, *jv);
      ++j; ++jv;
    }
  }
  return hash;
}
