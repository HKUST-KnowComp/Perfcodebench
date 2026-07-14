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

  // Find the range of keys to build a lookup table
  uint32_t max_key = 0;
  for (uint32_t k : keys) {
    if (k > max_key) max_key = k;
  }

  // Create a lookup table for O(1) access. 
  // Since keys are sorted (implied by lower_bound usage), 
  // we can map each possible key to its corresponding value.
  // If keys are not contiguous, we need to handle the gaps.
  // However, the baseline used lower_bound, which finds the first element >= probe.
  std::vector<uint32_t> lookup(max_key + 1);
  
  size_t current_val_idx = 0;
  for (uint32_t i = 0; i <= max_key; ++i) {
    while (current_val_idx < keys.size() && keys[current_val_idx] < i) {
      current_val_idx++;
    }
    if (current_val_idx < keys.size()) {
      lookup[i] = values[current_val_idx];
    } else {
      // If probe > max(keys), lower_bound returns keys.end(), 
      // which would access values[keys.size()]. 
      // We must handle this boundary case carefully.
      lookup[i] = values[keys.size()]; // This is technically UB in baseline if values.size() == keys.size()
    }
  }

  // To strictly match std::lower_bound behavior for probes > max_key:
  // The baseline does: idx = it - keys.begin(); hash = mix(hash, values[idx]);
  // If probe > keys.back(), idx == keys.size().
  // We need to ensure values has an element at index keys.size() or handle it.
  // Given the context of "dense id remapping", we assume values.size() >= keys.size() + 1 
  // or that probes are within range. Let's use a safe access.
  
  uint64_t final_hash = 0;
  const uint32_t* lookup_ptr = lookup.data();
  const uint32_t* probes_ptr = probes.data();
  size_t probes_size = probes.size();
  uint32_t max_lookup_idx = max_key;
  uint32_t fallback_val = (values.size() > keys.size()) ? values[keys.size()] : 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < probes_size; ++i) {
      uint32_t probe = probes_ptr[i];
      uint32_t val = (probe <= max_lookup_idx) ? lookup_ptr[probe] : fallback_val;
      hash = mix(hash, val);
    }
    final_hash = hash;
  }

  return final_hash;
}