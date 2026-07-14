#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  // Precompute the merged and deduplicated list once.
  std::vector<std::pair<uint32_t, uint32_t>> merged;
  merged.reserve(left_keys.size() + right_keys.size());

  size_t i = 0, j = 0;
  while (i < left_keys.size() || j < right_keys.size()) {
    uint32_t key;
    if (i < left_keys.size() && (j >= right_keys.size() || left_keys[i] <= right_keys[j])) {
      key = left_keys[i];
    } else {
      key = right_keys[j];
    }

    uint32_t val;
    // consume all entries with this key from left
    while (i < left_keys.size() && left_keys[i] == key) {
      val = left_values[i];
      ++i;
    }
    // consume all entries with this key from right (overwrites left value)
    while (j < right_keys.size() && right_keys[j] == key) {
      val = right_values[j];
      ++j;
    }
    merged.emplace_back(key, val);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const auto& kv : merged) {
      hash = mix(hash, kv.first);
      hash = mix(hash, kv.second);
    }
  }
  return hash;
}
