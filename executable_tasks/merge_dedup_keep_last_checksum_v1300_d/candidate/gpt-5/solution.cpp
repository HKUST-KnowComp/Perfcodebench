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

  const uint32_t* lks = nL ? left_keys.data() : nullptr;
  const uint32_t* lvs = nL ? left_values.data() : nullptr;
  const uint32_t* rks = nR ? right_keys.data() : nullptr;
  const uint32_t* rvs = nR ? right_values.data() : nullptr;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    std::size_t j = 0;
    hash = 1469598103934665603ULL;

    // Merge while both have remaining elements
    while (i < nL && j < nR) {
      uint32_t lk = lks[i];
      uint32_t rk = rks[j];
      if (lk < rk) {
        // Emit last occurrence from left for key lk
        uint32_t key = lk;
        uint32_t val = 0;
        do {
          val = lvs[i];
          ++i;
        } while (i < nL && lks[i] == key);
        hash = mix(hash, key);
        hash = mix(hash, val);
      } else if (rk < lk) {
        // Emit last occurrence from right for key rk
        uint32_t key = rk;
        uint32_t val = 0;
        do {
          val = rvs[j];
          ++j;
        } while (j < nR && rks[j] == key);
        hash = mix(hash, key);
        hash = mix(hash, val);
      } else {
        // Keys equal: keep last from right; skip all duplicates on both sides
        uint32_t key = lk;  // == rk
        // Skip left duplicates (value not used when right exists)
        do {
          ++i;
        } while (i < nL && lks[i] == key);
        // Skip right duplicates, remember last right value
        uint32_t valR = 0;
        do {
          valR = rvs[j];
          ++j;
        } while (j < nR && rks[j] == key);
        hash = mix(hash, key);
        hash = mix(hash, valR);
      }
    }

    // Drain remaining left run
    while (i < nL) {
      uint32_t key = lks[i];
      uint32_t val = 0;
      do {
        val = lvs[i];
        ++i;
      } while (i < nL && lks[i] == key);
      hash = mix(hash, key);
      hash = mix(hash, val);
    }

    // Drain remaining right run
    while (j < nR) {
      uint32_t key = rks[j];
      uint32_t val = 0;
      do {
        val = rvs[j];
        ++j;
      } while (j < nR && rks[j] == key);
      hash = mix(hash, key);
      hash = mix(hash, val);
    }
  }

  return hash;
}
