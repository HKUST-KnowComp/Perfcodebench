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
  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();
  const std::size_t ln = left_keys.size();
  const std::size_t rn = right_keys.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    std::size_t j = 0;
    uint64_t h = 1469598103934665603ULL; // FNV-1a offset basis

    while (i < ln || j < rn) {
      if (i < ln && (j >= rn || lk[i] < rk[j])) {
        // Consume a group of equal keys from left only
        uint32_t key = lk[i];
        uint32_t val = lv[i];
        ++i;
        while (i < ln && lk[i] == key) {
          val = lv[i];
          ++i;
        }
        h = mix(h, (uint64_t)key);
        h = mix(h, (uint64_t)val);
      } else if (j < rn && (i >= ln || rk[j] < lk[i])) {
        // Consume a group of equal keys from right only
        uint32_t key = rk[j];
        uint32_t val = rv[j];
        ++j;
        while (j < rn && rk[j] == key) {
          val = rv[j];
          ++j;
        }
        h = mix(h, (uint64_t)key);
        h = mix(h, (uint64_t)val);
      } else {
        // lk[i] == rk[j], consume both groups; keep last from right
        uint32_t key = lk[i];

        // Consume all left duplicates for this key
        ++i;
        while (i < ln && lk[i] == key) {
          ++i;
        }

        // Consume all right duplicates for this key, track last value
        uint32_t val_right = rv[j];
        ++j;
        while (j < rn && rk[j] == key) {
          val_right = rv[j];
          ++j;
        }

        h = mix(h, (uint64_t)key);
        h = mix(h, (uint64_t)val_right);
      }
    }

    hash = h; // store result of this iteration
  }

  return hash;
}
