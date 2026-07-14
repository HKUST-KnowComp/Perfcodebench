#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
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

  // Pre‑compute the merged (deduplicated, keep‑last) vector
  std::vector<std::pair<uint32_t, uint32_t>> merged;
  const std::size_t left_size = left_keys.size();
  const std::size_t right_size = right_keys.size();
  merged.reserve(left_size + right_size);

  std::size_t i = 0, j = 0;
  while (i < left_size && j < right_size) {
    if (left_keys[i] < right_keys[j]) {
      merged.emplace_back(left_keys[i], left_values[i]);
      ++i;
    } else if (right_keys[j] < left_keys[i]) {
      merged.emplace_back(right_keys[j], right_values[j]);
      ++j;
    } else {
      // keys equal: keep right value
      merged.emplace_back(right_keys[j], right_values[j]);
      ++i;
      ++j;
    }
  }
  while (i < left_size) {
    merged.emplace_back(left_keys[i], left_values[i]);
    ++i;
  }
  while (j < right_size) {
    merged.emplace_back(right_keys[j], right_values[j]);
    ++j;
  }

  // Repeatedly compute the FNV‑1a hash of the merged vector
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
