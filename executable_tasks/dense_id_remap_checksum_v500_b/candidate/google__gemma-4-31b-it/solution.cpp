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

  // Pre-calculate the mapping for all possible keys in the range
  // Since keys are sorted (implied by lower_bound usage in baseline),
  // we can fill the table linearly.
  std::vector<uint32_t> lookup(max_key + 1);
  size_t current_key_idx = 0;
  for (uint32_t i = 0; i <= max_key; ++i) {
    while (current_key_idx < keys.size() && keys[current_key_idx] < i) {
      current_key_idx++;
    }
    // The baseline used lower_bound, which returns the first element >= probe.
    // If probe > all keys, it returns keys.end(), and values[keys.size()] is accessed.
    // However, the baseline logic 'values[idx]' where idx = it - keys.begin()
    // suggests that values must be at least as large as keys.size() + 1 
    // or that probes are guaranteed to be within range. 
    // To be safe and match lower_bound exactly:
    if (current_key_idx < keys.size()) {
      lookup[i] = values[current_key_idx];
    } else {
      // This handles the case where probe > all keys in the keys vector
      // The baseline would access values[keys.size()], which is UB unless values is larger.
      // Given the task context, we assume values.size() >= keys.size() + 1 if this occurs.
      lookup[i] = values[keys.size()];
    }
  }

  uint64_t final_hash = 0;
  const uint32_t* probe_ptr = probes.data();
  const uint32_t* lookup_ptr = lookup.data();
  size_t probe_count = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < probe_count; ++i) {
      uint32_t probe = probe_ptr[i];
      // Handle probes that might exceed the pre-calculated lookup table range
      uint32_t val = (probe <= max_key) ? lookup_ptr[probe] : values[keys.size()];
      hash = mix(hash, val);
    }
    final_hash = hash;
  }

  return final_hash;
}