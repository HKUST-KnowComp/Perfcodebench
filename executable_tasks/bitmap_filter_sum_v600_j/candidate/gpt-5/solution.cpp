#include "interface.h"

#include <cstdint>
#include <vector>
#include <unordered_set>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  (void)iters; // The baseline recomputed the same result each iteration and returned the last one.

  const std::size_t n = probe_keys.size();
  if (n == 0) return 0ULL;

  uint64_t sum = 0ULL;

  // If key_space is zero, no in-range keys exist; fall back to set of all enabled keys.
  if (key_space == 0u) {
    if (enabled_keys.empty()) return 0ULL;
    std::unordered_set<uint32_t> enabled_set;
    enabled_set.reserve(enabled_keys.size() * 2);
    for (uint32_t k : enabled_keys) enabled_set.insert(k);
    for (std::size_t i = 0; i < n; ++i) {
      if (enabled_set.find(probe_keys[i]) != enabled_set.end()) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
    return sum;
  }

  // Build bitmap for in-range enabled keys and collect out-of-range keys if any.
  std::vector<uint8_t> bitmap(key_space, 0u);
  std::vector<uint32_t> overflow_keys;
  overflow_keys.reserve(0);
  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      bitmap[k] = 1u;
    } else {
      overflow_keys.push_back(k);
    }
  }

  if (overflow_keys.empty()) {
    // Fast path: all enabled keys are in range; no need for a hash set.
    const uint8_t* bm = bitmap.data();
    const uint32_t ks = key_space;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t pk = probe_keys[i];
      if (pk < ks && bm[pk]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  } else {
    // Build a set only for out-of-range enabled keys.
    std::unordered_set<uint32_t> overflow_set;
    overflow_set.reserve(overflow_keys.size() * 2);
    for (uint32_t k : overflow_keys) overflow_set.insert(k);

    const uint8_t* bm = bitmap.data();
    const uint32_t ks = key_space;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t pk = probe_keys[i];
      if (pk < ks) {
        if (bm[pk]) sum += static_cast<uint64_t>(values[i]);
      } else {
        if (overflow_set.find(pk) != overflow_set.end()) {
          sum += static_cast<uint64_t>(values[i]);
        }
      }
    }
  }

  return sum;
}
