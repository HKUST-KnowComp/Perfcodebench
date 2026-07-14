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
  uint32_t min_key = keys.front();
  uint32_t max_key = keys.back();
  
  // The problem implies dense IDs. We create a lookup table for the range [min_key, max_key].
  // Since keys are sorted (implied by lower_bound usage), we can map them linearly.
  std::vector<uint32_t> lookup(max_key - min_key + 1);
  
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i] - min_key] = values[i];
  }

  uint64_t final_hash = 0;
  const uint32_t* probe_ptr = probes.data();
  size_t probe_size = probes.size();
  const uint32_t* lookup_ptr = lookup.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t probe = probe_ptr[i];
      // The original code used lower_bound, which returns the first element >= probe.
      // If probe > max_key, it returns keys.end(), and values[keys.size()] is accessed (UB).
      // Assuming probes are within [min_key, max_key] based on "dense id" context.
      // To strictly mimic lower_bound for values outside the range:
      if (probe <= max_key) {
        if (probe < min_key) {
          hash = mix(hash, values[0]);
        } else {
          hash = mix(hash, lookup_ptr[probe - min_key]);
        }
      } else {
        // This case corresponds to it == keys.end(), which is values[keys.size()] in baseline.
        // However, in a real scenario, this is UB. We follow the logic of the baseline's index.
        hash = mix(hash, values[keys.size()]); 
      }
    }
    final_hash = hash;
  }
  return final_hash;
}
