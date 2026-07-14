#include "interface.h"

#include <cstdint>
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
  std::size_t n = left_keys.size();
  std::size_t m = right_keys.size();

  std::vector<std::pair<uint32_t, uint32_t>> merged;
  merged.reserve(n + m);

  std::size_t i = 0, j = 0;
  while (i < n && j < m) {
    uint32_t lk = left_keys[i];
    uint32_t rk = right_keys[j];
    if (lk < rk) {
      merged.emplace_back(lk, left_values[i]);
      ++i;
    } else if (lk > rk) {
      merged.emplace_back(rk, right_values[j]);
      ++j;
    } else {
      // Equal keys: keep the right-side value (last)
      merged.emplace_back(rk, right_values[j]);
      ++i;
      ++j;
    }
  }
  while (i < n) {
    merged.emplace_back(left_keys[i], left_values[i]);
    ++i;
  }
  while (j < m) {
    merged.emplace_back(right_keys[j], right_values[j]);
    ++j;
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