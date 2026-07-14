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

  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    std::size_t j = 0;
    uint64_t h = 1469598103934665603ULL;

    // Merge while both have remaining keys
    while (i < nL && j < nR) {
      // Left group: collapse duplicates, keep last value
      const uint32_t keyL = lk[i];
      std::size_t iEnd = i;
      while (iEnd + 1 < nL && lk[iEnd + 1] == keyL) {
        ++iEnd;
      }
      const uint32_t valL = lv[iEnd];

      // Right group: collapse duplicates, keep last value
      const uint32_t keyR = rk[j];
      std::size_t jEnd = j;
      while (jEnd + 1 < nR && rk[jEnd + 1] == keyR) {
        ++jEnd;
      }
      const uint32_t valR = rv[jEnd];

      if (keyL < keyR) {
        h = mix(h, keyL);
        h = mix(h, valL);
        i = iEnd + 1;
      } else if (keyR < keyL) {
        h = mix(h, keyR);
        h = mix(h, valR);
        j = jEnd + 1;
      } else { // keyL == keyR, keep right-side last value
        h = mix(h, keyL);
        h = mix(h, valR);
        i = iEnd + 1;
        j = jEnd + 1;
      }
    }

    // Drain remaining left groups
    while (i < nL) {
      const uint32_t keyL = lk[i];
      std::size_t iEnd = i;
      while (iEnd + 1 < nL && lk[iEnd + 1] == keyL) {
        ++iEnd;
      }
      const uint32_t valL = lv[iEnd];
      h = mix(h, keyL);
      h = mix(h, valL);
      i = iEnd + 1;
    }

    // Drain remaining right groups
    while (j < nR) {
      const uint32_t keyR = rk[j];
      std::size_t jEnd = j;
      while (jEnd + 1 < nR && rk[jEnd + 1] == keyR) {
        ++jEnd;
      }
      const uint32_t valR = rv[jEnd];
      h = mix(h, keyR);
      h = mix(h, valR);
      j = jEnd + 1;
    }

    hash = h; // return hash from the last iteration
  }

  return hash;
}