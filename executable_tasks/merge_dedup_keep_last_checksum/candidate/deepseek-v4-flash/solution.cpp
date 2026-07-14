#include "interface.h"

#include <cstdint>
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
  // Precompute merged result once (linear merge with dedup keep-last)
  std::vector<uint32_t> merged_keys;
  std::vector<uint32_t> merged_values;
  merged_keys.reserve(left_keys.size() + right_keys.size());
  merged_values.reserve(left_keys.size() + right_keys.size());

  std::size_t i = 0, j = 0;
  const std::size_t left_size = left_keys.size();
  const std::size_t right_size = right_keys.size();

  while (i < left_size && j < right_size) {
    uint32_t lk = left_keys[i];
    uint32_t rk = right_keys[j];
    if (lk < rk) {
      // Emit last left value for this key (never appears in right)
      uint32_t val = left_values[i];
      while (++i < left_size && left_keys[i] == lk) {
        val = left_values[i];
      }
      merged_keys.push_back(lk);
      merged_values.push_back(val);
    } else if (lk > rk) {
      // Emit last right value for this key (never appears in left)
      uint32_t val = right_values[j];
      while (++j < right_size && right_keys[j] == rk) {
        val = right_values[j];
      }
      merged_keys.push_back(rk);
      merged_values.push_back(val);
    } else {  // equal keys: right wins
      // Skip all left entries with this key
      while (++i < left_size && left_keys[i] == lk) {}
      // Take last right value for this key
      uint32_t val = right_values[j];
      while (++j < right_size && right_keys[j] == rk) {
        val = right_values[j];
      }
      merged_keys.push_back(rk);
      merged_values.push_back(val);
    }
  }

  // Remaining left entries (each unique key once, last value)
  while (i < left_size) {
    uint32_t key = left_keys[i];
    uint32_t val = left_values[i];
    while (++i < left_size && left_keys[i] == key) {
      val = left_values[i];
    }
    merged_keys.push_back(key);
    merged_values.push_back(val);
  }

  // Remaining right entries
  while (j < right_size) {
    uint32_t key = right_keys[j];
    uint32_t val = right_values[j];
    while (++j < right_size && right_keys[j] == key) {
      val = right_values[j];
    }
    merged_keys.push_back(key);
    merged_values.push_back(val);
  }

  const std::size_t merged_size = merged_keys.size();

  // Compute hash iters times over the precomputed result
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t k = 0; k < merged_size; ++k) {
      h = mix(h, merged_keys[k]);
      h = mix(h, merged_values[k]);
    }
    hash = h;
  }
  return hash;
}