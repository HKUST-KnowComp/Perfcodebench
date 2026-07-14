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
  const size_t ln = left_keys.size();
  const size_t rn = right_keys.size();
  const uint32_t* lk = ln ? left_keys.data() : nullptr;
  const uint32_t* lv = ln ? left_values.data() : nullptr;
  const uint32_t* rk = rn ? right_keys.data() : nullptr;
  const uint32_t* rv = rn ? right_values.data() : nullptr;

  // Pre-allocate buffer for merged keys and values
  std::vector<uint32_t> merged_keys;
  std::vector<uint32_t> merged_vals;
  merged_keys.reserve(ln + rn);
  merged_vals.reserve(ln + rn);

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    merged_keys.clear();
    merged_vals.clear();

    size_t i = 0, j = 0;

    // Two-pointer merge: right wins on duplicate keys (keep-last semantics)
    while (i < ln && j < rn) {
      uint32_t lkey = lk[i];
      uint32_t rkey = rk[j];
      if (lkey < rkey) {
        // Check if this key duplicates the last merged key
        if (!merged_keys.empty() && merged_keys.back() == lkey) {
          merged_vals.back() = lv[i];
        } else {
          merged_keys.push_back(lkey);
          merged_vals.push_back(lv[i]);
        }
        ++i;
      } else if (rkey < lkey) {
        if (!merged_keys.empty() && merged_keys.back() == rkey) {
          merged_vals.back() = rv[j];
        } else {
          merged_keys.push_back(rkey);
          merged_vals.push_back(rv[j]);
        }
        ++j;
      } else {
        // Equal keys: right value wins (keep-last), advance both
        if (!merged_keys.empty() && merged_keys.back() == lkey) {
          merged_vals.back() = rv[j];
        } else {
          merged_keys.push_back(lkey);
          merged_vals.push_back(rv[j]);
        }
        ++i;
        ++j;
      }
    }

    // Drain remaining left
    while (i < ln) {
      uint32_t lkey = lk[i];
      if (!merged_keys.empty() && merged_keys.back() == lkey) {
        merged_vals.back() = lv[i];
      } else {
        merged_keys.push_back(lkey);
        merged_vals.push_back(lv[i]);
      }
      ++i;
    }

    // Drain remaining right
    while (j < rn) {
      uint32_t rkey = rk[j];
      if (!merged_keys.empty() && merged_keys.back() == rkey) {
        merged_vals.back() = rv[j];
      } else {
        merged_keys.push_back(rkey);
        merged_vals.push_back(rv[j]);
      }
      ++j;
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    const size_t mn = merged_keys.size();
    const uint32_t* mk = merged_keys.data();
    const uint32_t* mv = merged_vals.data();
    for (size_t k = 0; k < mn; ++k) {
      hash = mix(hash, mk[k]);
      hash = mix(hash, mv[k]);
    }
  }

  return hash;
}
