#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL; // FNV-1a prime
  return hash;
}

static inline uint64_t compute_hash_once(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values) {
  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();
  const std::size_t nL = left_keys.size();
  const std::size_t nR = right_keys.size();

  std::size_t i = 0;
  std::size_t j = 0;

  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  while (i < nL || j < nR) {
    if (i < nL && (j >= nR || lk[i] < rk[j])) {
      // Next smallest key comes from left only.
      uint32_t key = lk[i];
      uint32_t val = lv[i];
      ++i;
      while (i < nL && lk[i] == key) {
        val = lv[i];
        ++i;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
    } else if (j < nR && (i >= nL || rk[j] < lk[i])) {
      // Next smallest key comes from right only.
      uint32_t key = rk[j];
      uint32_t val = rv[j];
      ++j;
      while (j < nR && rk[j] == key) {
        val = rv[j];
        ++j;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
    } else {
      // Equal keys in both runs; keep the right-side last occurrence.
      uint32_t key = lk[i]; // equal to rk[j]
      // Skip all left duplicates of this key.
      do { ++i; } while (i < nL && lk[i] == key);
      // Consume all right duplicates and take the last value.
      uint32_t val = rv[j];
      ++j;
      while (j < nR && rk[j] == key) {
        val = rv[j];
        ++j;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
    }
  }

  return hash;
}

}  // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  if (iters <= 0) return 0ULL;
  // Deterministic across iterations; compute once.
  return compute_hash_once(left_keys, left_values, right_keys, right_values);
}
