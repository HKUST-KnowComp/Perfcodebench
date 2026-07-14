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
  if (iters <= 0) return 0ULL;

  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();
  std::size_t ln = left_keys.size();
  std::size_t rn = right_keys.size();

  std::size_t i = 0, j = 0;
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  while (i < ln && j < rn) {
    uint32_t lkey = lk[i];
    uint32_t rkey = rk[j];
    if (lkey < rkey) {
      // Emit left key; collapse duplicates in left
      uint32_t key = lkey;
      uint32_t val = lv[i++];
      while (i < ln && lk[i] == key) {
        val = lv[i];
        ++i;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
    } else if (rkey < lkey) {
      // Emit right key; collapse duplicates in right
      uint32_t key = rkey;
      uint32_t val = rv[j++];
      while (j < rn && rk[j] == key) {
        val = rv[j];
        ++j;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
    } else {
      // Equal keys: consume both, keep last from right
      uint32_t key = lkey; // == rkey
      // Skip all left duplicates for this key
      while (i < ln && lk[i] == key) {
        ++i;
      }
      // Consume right duplicates and keep last value
      uint32_t val = rv[j++];
      while (j < rn && rk[j] == key) {
        val = rv[j];
        ++j;
      }
      hash = mix(hash, key);
      hash = mix(hash, val);
    }
  }

  // Remaining left-only keys
  while (i < ln) {
    uint32_t key = lk[i];
    uint32_t val = lv[i++];
    while (i < ln && lk[i] == key) {
      val = lv[i];
      ++i;
    }
    hash = mix(hash, key);
    hash = mix(hash, val);
  }

  // Remaining right-only keys
  while (j < rn) {
    uint32_t key = rk[j];
    uint32_t val = rv[j++];
    while (j < rn && rk[j] == key) {
      val = rv[j];
      ++j;
    }
    hash = mix(hash, key);
    hash = mix(hash, val);
  }

  return hash;
}
