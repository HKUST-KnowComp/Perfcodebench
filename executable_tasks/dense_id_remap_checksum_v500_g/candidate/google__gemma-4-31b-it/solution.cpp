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
  uint32_t max_key = keys.back();
  std::vector<uint32_t> lookup(max_key + 1);

  // Fill the lookup table. Since keys are sorted (implied by lower_bound usage),
  // we can fill the gaps to simulate lower_bound behavior.
  size_t current_val_idx = 0;
  for (uint32_t i = 0; i <= max_key; ++i) {
    if (current_val_idx < keys.size() && keys[current_val_idx] == i) {
      lookup[i] = values[current_val_idx];
      current_val_idx++;
    } else if (current_val_idx < keys.size()) {
      // This part handles the 'lower_bound' logic: 
      // if i is not in keys, it should map to the value of the first key > i
      lookup[i] = values[current_val_idx];
    } else {
      // If i > all keys, lower_bound returns keys.end(), which is index keys.size()
      // The original code accessed values[keys.size()], which is UB unless values is larger.
      // However, based on the baseline, we assume values[idx] is valid.
      lookup[i] = values[keys.size()];
    }
  }

  uint64_t final_hash = 0;
  const uint32_t* probe_ptr = probes.data();
  size_t probe_size = probes.size();
  const uint32_t* lookup_ptr = lookup.data();
  uint32_t lookup_limit = (uint32_t)lookup.size();
  uint32_t fallback_val = values[keys.size()];

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t p = probe_ptr[i];
      uint32_t val = (p < lookup_limit) ? lookup_ptr[p] : fallback_val;
      hash = mix(hash, val);
    }
    final_hash = hash;
  }
  return final_hash;
}