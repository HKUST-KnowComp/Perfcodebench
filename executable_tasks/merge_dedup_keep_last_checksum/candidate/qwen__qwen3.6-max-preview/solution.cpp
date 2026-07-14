#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  const size_t nL = left_keys.size();
  const size_t nR = right_keys.size();
  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t i = 0, j = 0;

    while (i < nL && j < nR) {
      uint32_t kL = lk[i];
      uint32_t kR = rk[j];
      if (kL < kR) {
        hash ^= kL; hash *= 1099511628211ULL;
        hash ^= lv[i]; hash *= 1099511628211ULL;
        ++i;
      } else if (kL > kR) {
        hash ^= kR; hash *= 1099511628211ULL;
        hash ^= rv[j]; hash *= 1099511628211ULL;
        ++j;
      } else {
        hash ^= kR; hash *= 1099511628211ULL;
        hash ^= rv[j]; hash *= 1099511628211ULL;
        ++i; ++j;
      }
    }
    while (i < nL) {
      hash ^= lk[i]; hash *= 1099511628211ULL;
      hash ^= lv[i]; hash *= 1099511628211ULL;
      ++i;
    }
    while (j < nR) {
      hash ^= rk[j]; hash *= 1099511628211ULL;
      hash ^= rv[j]; hash *= 1099511628211ULL;
      ++j;
    }
  }
  return hash;
}
