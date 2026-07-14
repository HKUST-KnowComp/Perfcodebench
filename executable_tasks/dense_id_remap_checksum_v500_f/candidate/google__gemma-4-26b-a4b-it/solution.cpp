#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  // Find the range of keys to build a direct lookup table.
  // We assume probes fall within a reasonable range or we handle them via binary search if out of bounds.
  // However, for 'dense_id_remap', a direct mapping table is the standard optimization.
  uint32_t max_key = keys.back();
  
  // Pre-calculate the remapped values for all possible keys in the range [0, max_key].
  // This turns the O(log N) search into an O(1) lookup.
  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i]] = values[i];
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      // The original code used std::lower_bound, which finds the first element >= probe.
      // To preserve exact correctness for non-exact matches, we must mimic lower_bound.
      // However, if the task implies 'dense' mapping where probes are keys, direct lookup is used.
      // To be 100% safe with the original logic (lower_bound behavior):
      auto it = std::lower_bound(keys.begin(), keys.end(), probe);
      const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
      hash = mix(hash, values[idx]);
    }
    final_hash = hash;
  }

  // RE-OPTIMIZATION STRATEGY:
  // The binary search is the killer. If we can't assume probes are exactly in keys,
  // we can still optimize by pre-calculating the result of lower_bound for all unique probes.
  // But even better: since 'iters' is high, we should compute the hash for one pass of 'probes'
  // and then realize the hash state evolves deterministically. 
  // Wait, the hash depends on the previous hash. The loop is: hash = mix(hash, val).
  // This is a sequential dependency. We cannot parallelize the 'iters' loop.
  // We CAN optimize the inner loop.

  // Let's use a more robust approach: Pre-calculate the remapped values for the specific probes.
  // Since 'probes' is constant across 'iters', we can pre-calculate the sequence of values.
  
  std::vector<uint32_t> remapped_values;
  remapped_values.reserve(probes.size());
  for (uint32_t probe : probes) {
    const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
    const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
    remapped_values.push_back(values[idx]);
  }

  uint64_t current_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    current_hash = 1469598103934665603ULL;
    for (uint32_t val : remapped_values) {
      current_hash = mix(current_hash, val);
    }
  }

  return current_hash;
}