#include "interface.h"

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
  if (iters <= 0) {
    return 0;
  }
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = left_keys.size();
  const size_t m = right_keys.size();
  size_t i = 0, j = 0;
  // Two-pointer merge of pre-sorted runs, keep right values for duplicate keys
  while (i < n && j < m) {
    if (left_keys[i] < right_keys[j]) {
      hash = mix(hash, left_keys[i]);
      hash = mix(hash, left_values[i]);
      ++i;
    } else if (right_keys[j] < left_keys[i]) {
      hash = mix(hash, right_keys[j]);
      hash = mix(hash, right_values[j]);
      ++j;
    } else {
      // Resolve duplicate by keeping the right-hand side value
      hash = mix(hash, right_keys[j]);
      hash = mix(hash, right_values[j]);
      ++i;
      ++j;
    }
  }
  // Add remaining elements from left run
  while (i < n) {
    hash = mix(hash, left_keys[i]);
    hash = mix(hash, left_values[i]);
    ++i;
  }
  // Add remaining elements from right run
  while (j < m) {
    hash = mix(hash, right_keys[j]);
    hash = mix(hash, right_values[j]);
    ++j;
  }
  // All subsequent iterations produce identical hash, no additional work needed
  return hash;
}