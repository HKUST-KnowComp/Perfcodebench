#include "interface.h"

#include <cstddef>
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
  // Merge and dedup in one pass (two-pointer merge with keep-last semantics).
  // Both input runs are assumed sorted (non-decreasing).
  std::vector<uint32_t> merged_keys;
  std::vector<uint32_t> merged_values;
  merged_keys.reserve(left_keys.size() + right_keys.size());  // upper bound
  merged_values.reserve(left_keys.size() + right_keys.size());

  std::size_t i = 0, j = 0;
  const std::size_t left_size = left_keys.size();
  const std::size_t right_size = right_keys.size();

  while (i < left_size || j < right_size) {
    // Determine next element in stable sorted order (left before right on equal keys).
    bool take_left;
    if (j == right_size) {
      take_left = true;
    } else if (i == left_size) {
      take_left = false;
    } else {
      take_left = left_keys[i] <= right_keys[j];
    }

    uint32_t key;
    uint32_t value;
    if (take_left) {
      key = left_keys[i];
      value = left_values[i];
      ++i;
    } else {
      key = right_keys[j];
      value = right_values[j];
      ++j;
    }

    // Dedup: if last key equals current, update its value; otherwise append.
    if (!merged_keys.empty() && merged_keys.back() == key) {
      merged_values.back() = value;
    } else {
      merged_keys.push_back(key);
      merged_values.push_back(value);
    }
  }

  // Compute checksum (FNV-1a) over merged result, repeated `iters` times.
  uint64_t hash = 0;
  const uint64_t fnv_offset = 1469598103934665603ULL;
  const std::size_t merged_size = merged_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    for (std::size_t k = 0; k < merged_size; ++k) {
      hash = mix(hash, merged_keys[k]);
      hash = mix(hash, merged_values[k]);
    }
  }
  return hash;
}
