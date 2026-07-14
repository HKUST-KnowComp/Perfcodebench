#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  // Find the maximum key to determine the size of the lookup table
  uint32_t max_key = 0;
  for (uint32_t k : keys) {
    if (k > max_key) max_key = k;
  }

  // Create a lookup table for O(1) access. 
  // Since keys are sorted (implied by lower_bound usage), we can fill this efficiently.
  std::vector<uint32_t> lookup(max_key + 1);
  size_t current_key_idx = 0;
  for (uint32_t i = 0; i <= max_key; ++i) {
    while (current_key_idx < keys.size() && keys[current_key_idx] < i) {
      current_key_idx++;
    }
    // The baseline used lower_bound, which returns the first element NOT LESS than probe.
    // If i is greater than all keys, it returns keys.end() (index = keys.size()).
    if (current_key_idx < keys.size()) {
      lookup[i] = values[current_key_idx];
    } else {
      // This case handles probes that exceed the maximum key in the keys vector
      // However, the lookup table only goes up to max_key. 
      // Probes > max_key will be handled separately in the loop.
      lookup[i] = values[keys.size()]; // This is technically out of bounds for values if values.size() == keys.size()
    }
  }

  // To handle the case where values.size() might be keys.size() + 1 (for the end iterator case)
  // we need to be careful. The baseline: idx = it - keys.begin(). 
  // If it == keys.end(), idx == keys.size().
  uint32_t default_val = (values.size() > keys.size()) ? values[keys.size()] : 0;

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      uint32_t val;
      if (probe <= max_key) {
        val = lookup[probe];
      } else {
        val = default_val;
      }
      hash = mix(hash, val);
    }
    final_hash = hash;
  }
  return final_hash;
}