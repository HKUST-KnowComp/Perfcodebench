#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();
  const std::size_t nL = left_keys.size();
  const std::size_t nR = right_keys.size();

  std::size_t i = 0, j = 0;
  uint64_t hash = 1469598103934665603ULL; // FNV offset basis

  // Merge two sorted runs, collapsing duplicates within each run (keep last),
  // and preferring right on cross-run duplicate keys.
  while (i < nL && j < nR) {
    uint32_t li_key = lk[i];
    uint32_t rj_key = rk[j];
    if (li_key < rj_key) {
      // Consume left group for li_key
      std::size_t i_end = i + 1;
      while (i_end < nL && lk[i_end] == li_key) ++i_end;
      uint32_t val = lv[i_end - 1]; // last occurrence within left group
      hash = mix(hash, li_key);
      hash = mix(hash, val);
      i = i_end;
    } else if (li_key > rj_key) {
      // Consume right group for rj_key
      std::size_t j_end = j + 1;
      while (j_end < nR && rk[j_end] == rj_key) ++j_end;
      uint32_t val = rv[j_end - 1]; // last occurrence within right group
      hash = mix(hash, rj_key);
      hash = mix(hash, val);
      j = j_end;
    } else {
      // Same key on both sides; right wins. Consume both groups.
      std::size_t i_end = i + 1;
      while (i_end < nL && lk[i_end] == li_key) ++i_end;
      std::size_t j_end = j + 1;
      while (j_end < nR && rk[j_end] == rj_key) ++j_end;
      uint32_t val = rv[j_end - 1];
      hash = mix(hash, li_key);
      hash = mix(hash, val);
      i = i_end;
      j = j_end;
    }
  }

  // Consume remaining groups from left
  while (i < nL) {
    uint32_t key = lk[i];
    std::size_t i_end = i + 1;
    while (i_end < nL && lk[i_end] == key) ++i_end;
    uint32_t val = lv[i_end - 1];
    hash = mix(hash, key);
    hash = mix(hash, val);
    i = i_end;
  }

  // Consume remaining groups from right
  while (j < nR) {
    uint32_t key = rk[j];
    std::size_t j_end = j + 1;
    while (j_end < nR && rk[j_end] == key) ++j_end;
    uint32_t val = rv[j_end - 1];
    hash = mix(hash, key);
    hash = mix(hash, val);
    j = j_end;
  }

  return hash;
}
