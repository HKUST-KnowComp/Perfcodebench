#include "interface.h"

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
  const std::size_t nL = left_keys.size();
  const std::size_t nR = right_keys.size();

  const uint32_t* LK = left_keys.data();
  const uint32_t* LV = left_values.data();
  const uint32_t* RK = right_keys.data();
  const uint32_t* RV = right_values.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0, j = 0;
    hash = 1469598103934665603ULL;  // FNV offset basis

    while (i < nL || j < nR) {
      if (j >= nR) {
        // Only left remains
        uint32_t key = LK[i];
        // Advance i to consume all duplicates of this key in left
        std::size_t ie = i + 1;
        while (ie < nL && LK[ie] == key) ++ie;
        uint32_t val = LV[ie - 1];
        i = ie;
        hash = mix(hash, key);
        hash = mix(hash, val);
      } else if (i >= nL) {
        // Only right remains
        uint32_t key = RK[j];
        std::size_t je = j + 1;
        while (je < nR && RK[je] == key) ++je;
        uint32_t val = RV[je - 1];
        j = je;
        hash = mix(hash, key);
        hash = mix(hash, val);
      } else {
        uint32_t lk = LK[i];
        uint32_t rk = RK[j];
        if (lk < rk) {
          // Emit left key's last value for this key
          uint32_t key = lk;
          std::size_t ie = i + 1;
          while (ie < nL && LK[ie] == key) ++ie;
          uint32_t val = LV[ie - 1];
          i = ie;
          hash = mix(hash, key);
          hash = mix(hash, val);
        } else if (rk < lk) {
          // Emit right key's last value for this key
          uint32_t key = rk;
          std::size_t je = j + 1;
          while (je < nR && RK[je] == key) ++je;
          uint32_t val = RV[je - 1];
          j = je;
          hash = mix(hash, key);
          hash = mix(hash, val);
        } else {
          // Keys equal in both runs: keep-right (last of right duplicates)
          uint32_t key = lk;  // same as rk
          // Skip all left duplicates of this key
          std::size_t ie = i + 1;
          while (ie < nL && LK[ie] == key) ++ie;
          // Skip all right duplicates of this key, capture last right value
          std::size_t je = j + 1;
          while (je < nR && RK[je] == key) ++je;
          uint32_t val = RV[je - 1];
          i = ie;
          j = je;
          hash = mix(hash, key);
          hash = mix(hash, val);
        }
      }
    }
  }

  return hash;
}
