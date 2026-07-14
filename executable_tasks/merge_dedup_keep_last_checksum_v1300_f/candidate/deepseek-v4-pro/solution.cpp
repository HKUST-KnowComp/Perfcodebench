// Optimized merge-dedup: linear two-pointer merge, keep last on duplicates, cache result
#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <utility>
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
  // No iterations requested: return 0 (matching original behaviour)
  if (iters <= 0) {
    return 0;
  }

  // Build the fully deduplicated, keep-last merged view *once*.
  // Both inputs are assumed sorted and individually duplicate-free.
  std::vector<std::pair<uint32_t, uint32_t>> out;
  out.reserve(left_keys.size() + right_keys.size());

  std::size_t i = 0, j = 0;
  while (i < left_keys.size() && j < right_keys.size()) {
    if (left_keys[i] < right_keys[j]) {
      out.emplace_back(left_keys[i], left_values[i]);
      ++i;
    } else if (left_keys[i] > right_keys[j]) {
      out.emplace_back(right_keys[j], right_values[j]);
      ++j;
    } else {  // equal key: keep right-side value, consume both
      out.emplace_back(right_keys[j], right_values[j]);
      ++i;
      ++j;
    }
  }
  while (i < left_keys.size()) {
    out.emplace_back(left_keys[i], left_values[i]);
    ++i;
  }
  while (j < right_keys.size()) {
    out.emplace_back(right_keys[j], right_values[j]);
    ++j;
  }

  // Loop exactly `iters` times, recomputing the hash from the same `out`.
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (const auto& kv : out) {
      h = mix(h, kv.first);
      h = mix(h, kv.second);
    }
    hash = h;  // keep last iteration's result
  }

  return hash;
}