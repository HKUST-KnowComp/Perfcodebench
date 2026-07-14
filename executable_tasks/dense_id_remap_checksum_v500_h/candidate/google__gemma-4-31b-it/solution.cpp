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

  // The baseline uses std::lower_bound, which finds the first element NOT less than probe.
  // Since keys are likely sorted (implied by lower_bound usage), we can build a lookup table.
  // We need to map every possible probe value to the value at the index returned by lower_bound.
  
  uint32_t max_key = keys.back();
  // We create a lookup table that maps each integer from 0 to max_key to the corresponding value.
  // If a probe is greater than max_key, lower_bound returns keys.end(), which is index keys.size().
  // However, the baseline uses values[idx], which would be out of bounds if idx == keys.size().
  // Assuming the problem constraints ensure probes are within [0, max_key] or the baseline
  // logic is safe, we build the table.
  
  std::vector<uint32_t> lookup(max_key + 1);
  size_t current_key_idx = 0;
  for (uint32_t i = 0; i <= max_key; ++i) {
    while (current_key_idx < keys.size() && keys[current_key_idx] < i) {
      current_key_idx++;
    }
    // Now keys[current_key_idx] >= i, which is exactly what lower_bound does.
    // If current_key_idx == keys.size(), this will crash, but the baseline would too.
    lookup[i] = values[current_key_idx];
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      // Handle probes larger than the max key in the lookup table
      uint32_t val = (probe <= max_key) ? lookup[probe] : values[keys.size()];
      hash = mix(hash, val);
    }
    final_hash = hash;
  }
  
  // The baseline resets hash inside the loop and returns the result of the last iteration.
  // Note: The baseline code provided actually returns the hash of the LAST iteration
  // because 'hash' is redeclared/reset inside the loop. 
  // Wait, the baseline says: uint64_t hash = 0; for(...) { hash = 1469...; ... }
  // This means only the last iteration's result is returned.
  
  return final_hash;
}